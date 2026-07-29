#!/usr/bin/env python3

import argparse
import collections
import re
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
JAVA_ROOT = ROOT / "jvm" / "src" / "main" / "java"
CPP_ROOT = ROOT / "plugins"


def java_descriptor(type_name: str, package: str, outer_class: str) -> str:
    if type_name.endswith("[]"):
        return "[" + java_descriptor(type_name[:-2], package, outer_class)
    primitive = {
        "boolean": "Z",
        "byte": "B",
        "double": "D",
        "float": "F",
        "int": "I",
        "long": "J",
        "void": "V",
    }
    if type_name in primitive:
        return primitive[type_name]
    if type_name == "String":
        return "Ljava/lang/String;"
    return f"L{package.replace('.', '/')}/{outer_class}${type_name};"


def jni_encode(value: str) -> str:
    result = []
    for character in value:
        if character == "/":
            result.append("_")
        elif character == "_":
            result.append("_1")
        elif character == ";":
            result.append("_2")
        elif character == "[":
            result.append("_3")
        elif character.isalnum():
            result.append(character)
        else:
            result.append(f"_0{ord(character):04x}")
    return "".join(result)


def parse_java(java_files):
    native_symbols = set()
    callback_descriptors = collections.defaultdict(dict)
    for path in java_files:
        source = path.read_text()
        package = re.search(r"^package ([\w.]+);", source, re.MULTILINE).group(1)
        outer_class = re.search(r"public final class (\w+)", source).group(1)
        binary_name = package.replace(".", "/") + "/" + outer_class
        for method in re.findall(r"public static native [\w\[\]]+ (\w+)\s*\(", source):
            native_symbols.add("Java_" + jni_encode(binary_name) + "_" + jni_encode(method))
        for callback, parameters in re.findall(
            r"public interface (\w+Callback)\s*\{\s*void invoke\(([^)]*)\);", source
        ):
            descriptor = ""
            if parameters.strip():
                for parameter in parameters.split(","):
                    type_name = parameter.strip().split()[0]
                    descriptor += java_descriptor(type_name, package, outer_class)
            callback_descriptors[path.parent.name][callback] = f"({descriptor})V"
    return java_files, native_symbols, callback_descriptors


def parse_cpp_callbacks():
    descriptors = collections.defaultdict(dict)
    pattern = re.compile(
        r"struct (\w+Callback)Wrapper\s*\{.*?"
        r'GetMethodID\(callbackClass, "invoke", "([^"]+)"\)',
        re.DOTALL,
    )
    for path in sorted(CPP_ROOT.glob("*/*_jni.cpp")):
        for callback, descriptor in pattern.findall(path.read_text()):
            descriptors[path.parent.name][callback] = descriptor
    return descriptors


def exported_symbols(library: Path):
    commands = [
        ["nm", "-gU", str(library)],
        ["nm", "-D", "--defined-only", str(library)],
    ]
    for command in commands:
        result = subprocess.run(command, text=True, capture_output=True)
        if result.returncode == 0:
            return {
                match.group(1).lstrip("_")
                for line in result.stdout.splitlines()
                if (match := re.search(r"(_?Java_\S+)$", line))
                and "Java_io_mavsdk_jni_" in match.group(1)
            }
    raise RuntimeError(f"Could not inspect native symbols in {library}")


def assert_shape(path: Path, *fragments: str):
    source = path.read_text()
    for fragment in fragments:
        assert fragment in source, f"{path}: missing representative shape {fragment!r}"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--library", type=Path, default=ROOT / "build" / "libmavsdk_jni.dylib"
    )
    args = parser.parse_args()

    java_files = sorted((JAVA_ROOT / "io/mavsdk/jni/plugins").rglob("Native*.java"))
    core_files = sorted((JAVA_ROOT / "io/mavsdk/jni").glob("Native*.java"))
    _, expected_symbols, java_callbacks = parse_java(java_files)
    _, core_symbols, _ = parse_java(core_files)
    expected_symbols |= core_symbols
    cpp_callbacks = parse_cpp_callbacks()
    assert len(java_files) == 36, f"expected 36 Java contracts, found {len(java_files)}"
    assert len(core_files) == 4, f"expected 4 core Java contracts, found {len(core_files)}"
    assert len(expected_symbols) == 699, (
        f"expected 699 Java native declarations, found {len(expected_symbols)}"
    )
    assert sum(map(len, java_callbacks.values())) == 215, "unexpected callback count"
    assert java_callbacks == cpp_callbacks, "Java/C++ callback descriptors differ"

    assert args.library.is_file(), f"native library not found: {args.library}"
    actual_symbols = exported_symbols(args.library)
    missing_symbols = sorted(expected_symbols - actual_symbols)
    extra_symbols = sorted(actual_symbols - expected_symbols)
    assert not missing_symbols and not extra_symbols, (
        f"native symbol mismatch; missing={missing_symbols[:5]}, extra={extra_symbols[:5]}"
    )

    assert_shape(
        JAVA_ROOT / "io/mavsdk/jni/plugins/telemetry/NativeTelemetry.java",
        "public final float[] controls;",
        "float[] controls",
    )
    assert_shape(
        JAVA_ROOT / "io/mavsdk/jni/plugins/ftp/NativeFtp.java",
        "public final String[] dirs;",
        "public final String[] files;",
    )
    assert_shape(
        JAVA_ROOT / "io/mavsdk/jni/plugins/mission_raw/NativeMissionRaw.java",
        "public static native MissionItem[] downloadMission",
        "void invoke(int result, MissionItem[] value);",
    )
    assert_shape(
        JAVA_ROOT / "io/mavsdk/jni/plugins/geofence/NativeGeofence.java",
        "public final Point[] points;",
        "public final Polygon[] polygons;",
        "public final Point point;",
    )
    assert_shape(
        JAVA_ROOT / "io/mavsdk/jni/plugins/camera/NativeCamera.java",
        "public final Option option;",
        "public final Option[] options;",
    )
    assert_shape(
        CPP_ROOT / "mission_raw/mission_raw_jni.cpp",
        "MissionItemArrayFromJava",
        "mavsdk_mission_raw_mission_item_array_destroy",
    )
    assert_shape(
        CPP_ROOT / "telemetry/telemetry_jni.cpp",
        "GetFloatArrayRegion",
        "SetFloatArrayRegion",
    )
    assert_shape(
        CPP_ROOT / "geofence/geofence_jni.cpp",
        "PointFromJava",
        "PolygonArrayFromJava",
        "toJavaPoint",
    )

    generated = [
        ROOT / "templates/file_jni.cpp.j2",
        ROOT / "templates/file_jni.java.j2",
        *CPP_ROOT.glob("*/*_jni.cpp"),
        *java_files,
        *core_files,
    ]
    forbidden = re.compile(
        r"\bTODO\b|placeholder|repeated param|nullptr,\s*0|static_cast<jobject>\(nullptr\)"
    )
    offenders = [str(path) for path in generated if forbidden.search(path.read_text())]
    assert not offenders, f"generated conversion placeholders remain: {offenders}"

    print(
        f"Validated {len(java_files)} plugin and {len(core_files)} core contracts, "
        f"{len(expected_symbols)} native symbols, "
        f"and {sum(map(len, java_callbacks.values()))} callback descriptors."
    )


if __name__ == "__main__":
    main()
