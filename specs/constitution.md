# Project constitution

Standards that hold across every feature of the drone-in-a-box dock controller.
Written 2026-10-15 from decisions already made in this project rather than from
a blank form; amend it when a decision changes.

## Context

Smart Solutions Semester Fall 2026–2027. A mobile drone-in-a-box mounted on a
vehicle roof. The semester deliverable is a **working proof of concept** plus a
project plan, requirements document, design document and test report.

Two audiences matter and they pull in different directions: the assessor, who
needs a demonstrable system and evidence of testing, and the client, who needs
an architecture that could become a product. Where they conflict, prefer the
demonstrable thing and *document* the production path.

## Tech stack

- **Target:** ESP32-S3, ESP-IDF **v6.0.2**, C17. Pin the IDF version in docs —
  IDF 6.0 split `driver` into `esp_driver_*` components, so a teammate on 5.x
  cannot build this.
- **Build:** `idf.py`. Host tests: plain CMake + CTest + Unity from the IDF tree.
- **No Arduino framework.** The sketches under `firmware/actuator_test/` are
  historical reference only.

## Non-negotiables

1. **Firmware is authoritative.** A client proposes, the firmware decides. No
   remote client is ever trusted to send the stop that protects the mechanism.
2. **Anything that moves needs a dead-man switch.** Any remote control transport
   must stop the actuator when the controlling client goes silent.
3. **No `delay()`, no busy-wait, no polling loops.** Block on interrupt-driven
   primitives or arm a timer. The CPU must be free between events.
4. **Hardware-free logic goes in a component with no ESP-IDF headers**, so it is
   unit testable on a host. Anything that can be got wrong by reasoning —
   arithmetic, parsing, bounds, encoding — belongs there.
5. **Secrets never enter git.** No credentials, keys or tokens in tracked files.
6. **`-Werror` stays on.** A warning is a defect that has not happened yet.

## Testing expectations

Three levels, kept distinct in the report:

| Level | Runs on | Lives in |
| --- | --- | --- |
| Unit | Host, no hardware | `firmware/*/test/host/` |
| Integration | Bench: firmware + driver + actuator | project `README.md` |
| System | Whole dock on the vehicle | `docs/testing/` |

Every regression test must be shown to fail against the bug it guards. A test
that has never failed is decoration.

## Constraints

- **Flash budget.** 1 MB app partition, currently 841 KB used. Any feature that
  pushes past it must come with a partition-table change and verification of the
  board's actual flash size — not an assumption.
- **ADC1 only.** Wi-Fi owns ADC2 on the S3, so GPIO1–10 are reserved for
  actuator position feedback. Avoid GPIO19/20 (native USB) and GPIO26–37
  (flash/PSRAM).
- **One semester.** Prefer a working subset with a documented path over an
  ambitious design that does not demo.
