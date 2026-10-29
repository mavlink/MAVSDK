#!/usr/bin/env python3
"""Run with Python 3 on macOS with Xcode and tree installed; no MAVSDK build needed."""

import hashlib
import os
from pathlib import Path
import plistlib
import shutil
import subprocess
import tempfile
import unittest
import zipfile


SCRIPT = Path(__file__).with_name("package_mavsdk_server_framework.bash")
BACKEND = Path("src/mavsdk_server/src")
DSYM = "mavsdk_server.framework.dSYM"
DWARF = Path("Contents/Resources/DWARF/mavsdk_server")
TARGETS = (
    ("ios", "iphoneos", "arm64-apple-ios14.0"),
    ("ios_simulator_x64", "iphonesimulator", "x86_64-apple-ios14.0-simulator"),
    ("ios_simulator_arm64", "iphonesimulator", "arm64-apple-ios14.0-simulator"),
    ("macos_x64", "macosx", "x86_64-apple-macos11.0"),
    ("macos_arm64", "macosx", "arm64-apple-macos11.0"),
)


def run(*args):
    return subprocess.check_output(args, text=True, stderr=subprocess.STDOUT).strip()


def uuids(path):
    return {
        tuple(line.split()[1:3])
        for line in run("xcrun", "dwarfdump", "--uuid", str(path)).splitlines()
    }


@unittest.skipUnless(os.uname().sysname == "Darwin", "Requires macOS and Xcode")
class FrameworkPackagingTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.fixtures = tempfile.TemporaryDirectory(prefix="mavsdk-framework-inputs-")
        cls.addClassCleanup(cls.fixtures.cleanup)
        cls.inputs = Path(cls.fixtures.name)
        source = cls.inputs / "fixture.c"
        source.write_text("int mavsdk_fixture(void) { return 42; }\n")
        for name, sdk, target in TARGETS:
            framework = cls.inputs / name / BACKEND / "mavsdk_server.framework"
            mac = sdk == "macosx"
            content = framework / "Versions/A" if mac else framework
            content.mkdir(parents=True)
            (content / "Headers").mkdir()
            (content / "Headers/mavsdk_server_api.h").write_text(
                "int mavsdk_fixture(void);\n"
            )
            (content / "Modules").mkdir()
            (content / "Modules/module.modulemap").write_text(
                'framework module mavsdk_server { umbrella header "mavsdk_server_api.h" export * }\n'
            )
            resources = content / "Resources" if mac else content
            resources.mkdir(exist_ok=True)
            (resources / "Info.plist").write_bytes(
                plistlib.dumps(
                    {
                        "CFBundleExecutable": "mavsdk_server",
                        "CFBundleIdentifier": "io.mavlink.mavsdk",
                        "CFBundleName": "mavsdk_server",
                        "CFBundlePackageType": "FMWK",
                        "CFBundleVersion": "1",
                        "CFBundleShortVersionString": "1.0.0",
                    }
                )
            )
            if mac:
                (framework / "Versions/Current").symlink_to("A")
                for entry in ("mavsdk_server", "Headers", "Modules", "Resources"):
                    (framework / entry).symlink_to(f"Versions/Current/{entry}")
            obj = cls.inputs / f"{name}.o"
            flags = (
                "-target",
                target,
                "-isysroot",
                run("xcrun", "--sdk", sdk, "--show-sdk-path"),
            )
            run("xcrun", "clang", "-g", *flags, "-c", str(source), "-o", str(obj))
            run(
                "xcrun",
                "clang",
                "-dynamiclib",
                *flags,
                str(obj),
                "-install_name",
                "@rpath/mavsdk_server.framework/mavsdk_server",
                "-o",
                str(content / "mavsdk_server"),
            )
            dsym = framework.with_name(DSYM)
            run("xcrun", "dsymutil", str(content / "mavsdk_server"), "-o", str(dsym))
            # GitHub artifact downloads do not preserve executable permissions.
            for path in dsym.rglob("*"):
                if path.is_file():
                    path.chmod(0o644)

    def setUp(self):
        temporary = tempfile.TemporaryDirectory(prefix="mavsdk-framework-package-")
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.build = self.root / "cpp/build"
        self.build.mkdir(parents=True)
        for name, _, _ in TARGETS:
            shutil.copytree(self.inputs / name, self.build / name, symlinks=True)
        self.script = self.root / "cpp/src/mavsdk_server/tools" / SCRIPT.name
        self.script.parent.mkdir(parents=True)
        shutil.copyfile(SCRIPT, self.script)
        self.xcframework = self.build / "mavsdk_server.xcframework"

    def package(self):
        return subprocess.run(
            ["bash", str(self.script)], text=True, capture_output=True
        )

    def test_all_slices_have_matching_symbols_and_valid_archive(self):
        result = self.package()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        info = plistlib.loads((self.xcframework / "Info.plist").read_bytes())
        libraries = {
            entry["LibraryIdentifier"]: entry for entry in info["AvailableLibraries"]
        }
        self.assertEqual(
            set(libraries),
            {
                "ios-arm64",
                "ios-arm64_x86_64-simulator",
                "macos-arm64_x86_64",
            },
        )
        for identifier, entry in libraries.items():
            with self.subTest(slice=identifier):
                root = self.xcframework / identifier
                binary = root / entry["LibraryPath"] / "mavsdk_server"
                symbols = root / entry["DebugSymbolsPath"] / DSYM / DWARF
                self.assertEqual(uuids(binary), uuids(symbols))
                self.assertEqual(
                    len(uuids(symbols)), len(entry["SupportedArchitectures"])
                )
                self.assertTrue(os.access(binary, os.X_OK))
        archive = self.build / "mavsdk_server.xcframework.zip"
        with zipfile.ZipFile(archive) as zipped:
            self.assertIsNone(zipped.testzip())
            for identifier in libraries:
                self.assertIn(
                    f"mavsdk_server.xcframework/{identifier}/dSYMs/{DSYM}/{DWARF}",
                    zipped.namelist(),
                )
        self.assertEqual(
            hashlib.sha256(archive.read_bytes()).hexdigest(),
            archive.with_suffix(".zip.sha256").read_text().strip(),
        )

    def test_missing_dsym_fails_before_creating_xcframework(self):
        symbols = self.build / "ios" / BACKEND / DSYM
        shutil.rmtree(symbols)
        result = self.package()
        self.assertNotEqual(result.returncode, 0)
        self.assertFalse(self.xcframework.exists())

    def test_missing_simulator_dsym_fails_before_creating_xcframework(self):
        shutil.rmtree(self.build / "ios_simulator_arm64" / BACKEND / DSYM)
        result = self.package()
        self.assertNotEqual(result.returncode, 0)
        self.assertFalse(self.xcframework.exists())

    def test_missing_macos_dsym_fails_before_creating_xcframework(self):
        shutil.rmtree(self.build / "macos_x64" / BACKEND / DSYM)
        result = self.package()
        self.assertNotEqual(result.returncode, 0)
        self.assertFalse(self.xcframework.exists())

    def test_mismatched_uuid_fails_before_creating_xcframework(self):
        symbols = self.build / "ios" / BACKEND / DSYM / DWARF
        shutil.copyfile(self.build / "macos_arm64" / BACKEND / DSYM / DWARF, symbols)
        result = self.package()
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Framework and dSYM UUIDs do not match", result.stderr)
        self.assertFalse(self.xcframework.exists())

    def test_mismatched_universal_uuid_fails_before_creating_xcframework(self):
        symbols = self.build / "ios_simulator_arm64" / BACKEND / DSYM / DWARF
        shutil.copyfile(self.build / "ios" / BACKEND / DSYM / DWARF, symbols)
        result = self.package()
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Framework and dSYM UUIDs do not match", result.stderr)
        self.assertFalse(self.xcframework.exists())

    def test_mismatched_macos_uuid_fails_before_creating_xcframework(self):
        symbols = self.build / "macos_arm64" / BACKEND / DSYM / DWARF
        shutil.copyfile(self.build / "ios" / BACKEND / DSYM / DWARF, symbols)
        result = self.package()
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Framework and dSYM UUIDs do not match", result.stderr)
        self.assertFalse(self.xcframework.exists())


if __name__ == "__main__":
    unittest.main()
