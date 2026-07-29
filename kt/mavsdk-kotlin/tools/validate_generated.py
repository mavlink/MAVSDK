#!/usr/bin/env python3

from pathlib import Path
import sys


ROOT = Path(__file__).resolve().parents[1]
COMMON = ROOT / "src/commonMain/kotlin/io/mavsdk/kotlin/plugins"
ACTUAL = ROOT / "src/jvmAndroidMain/kotlin/io/mavsdk/kotlin/plugins"
EXPECTED_PLUGIN_COUNT = 36


def fail(message: str) -> None:
    print(f"error: {message}", file=sys.stderr)
    raise SystemExit(1)


common_files = sorted(COMMON.glob("*/*.kt"))
actual_files = sorted(ACTUAL.glob("*/*Native.kt"))

if len(common_files) != EXPECTED_PLUGIN_COUNT:
    fail(f"expected {EXPECTED_PLUGIN_COUNT} common plugin files, found {len(common_files)}")
if len(actual_files) != EXPECTED_PLUGIN_COUNT:
    fail(f"expected {EXPECTED_PLUGIN_COUNT} JVM/Android adapters, found {len(actual_files)}")

common_plugins = {path.parent.name for path in common_files}
actual_plugins = {path.parent.name for path in actual_files}
if common_plugins != actual_plugins:
    fail(
        "common and JVM/Android plugin sets differ: "
        f"common-only={sorted(common_plugins - actual_plugins)}, "
        f"adapter-only={sorted(actual_plugins - common_plugins)}"
    )

generated_text = "\n".join(path.read_text() for path in common_files + actual_files)
for forbidden in ("TODO", "NotImplementedError", "external fun"):
    if forbidden in generated_text:
        fail(f"generated Kotlin contains {forbidden!r}")

representative_shapes = {
    COMMON / "mission_raw_server/MissionRawServer.kt": (
        "val missionItems: List<MissionItem> = emptyList()",
        "fun subscribeIncomingMission(): Flow<MissionPlan>",
    ),
    COMMON / "mission/Mission.kt": (
        "fun uploadMissionWithProgress(missionPlan: MissionPlan)",
        "fun downloadMissionWithProgress()",
    ),
    COMMON / "param/Param.kt": (
        "val intParams: List<IntParam> = emptyList()",
        "fun getAllParams(): AllParams",
    ),
    ACTUAL / "mission_raw_server/MissionRawServerNative.kt": (
        "missionItems.map { it.toNative() }.toTypedArray()",
        "missionItems.map { it.toKotlin() }",
    ),
}

for path, snippets in representative_shapes.items():
    text = path.read_text()
    for snippet in snippets:
        if snippet not in text:
            fail(f"{path.relative_to(ROOT)} is missing {snippet!r}")

print(
    f"Validated {len(common_files)} common plugin APIs and "
    f"{len(actual_files)} JVM/Android adapters."
)
