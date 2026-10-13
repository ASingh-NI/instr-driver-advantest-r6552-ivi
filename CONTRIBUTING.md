# Contributing to instr-driver-advantest-r6552-ivi

Contributions to the Advantest R6552 IVI-C driver are welcome from all!

instr-driver-advantest-r6552-ivi is managed via [git](https://git-scm.com), with the canonical upstream
repository hosted on [GitHub](https://github.com/ASingh-NI/instr-driver-advantest-r6552-ivi/).

instr-driver-advantest-r6552-ivi follows a pull-request model for development. To contribute, create a
GitHub account, fork this project, push a branch with your changes to your fork, and
then submit a pull request.

Please sign off your commits (e.g. `git commit -s`). This adds a line of the form
`Signed-off-by: Name Lastname <name.lastname@example.com>` to the commit message,
indicating that you have read and agree to the Developer Certificate of Origin (see
below). Include a `Signed-off-by` line for every author of a commit.

See [GitHub's documentation](https://docs.github.com/pull-requests) for more details.

# Getting Started

1. Install the prerequisites:
   - LabWindows/CVI.
   - IVI Compliance Package 4.6.1 or later and the IviDmm class specification.
   - [NI-VISA](https://www.ni.com/en/support/downloads/drivers/download.ni-visa.html)
     5.2 or later, and NI MAX.
   - LabVIEW (oldest version the wrapper is saved in), only if you change the LabVIEW
     wrapper.
   - [Uncrustify](https://github.com/uncrustify/uncrustify) for source formatting.
2. Fork this repository and clone your fork.
3. Create a topic branch, e.g. `git checkout -b users/<name>/avr6552-<change>`.
4. Open `avr6552/avr6552.prj` in LabWindows/CVI and build it for 32-bit and 64-bit to
   confirm a clean starting point.

# Making Changes

- Follow IVI-3.1, IVI-3.2, and the IviDmm class specification (IVI-4.2).
- Implement new settings as `AVR6552_ATTR_*` attributes with range tables and
  read/write/check callbacks, and expose them through `avr6552_Configure*` functions
  where appropriate. Remember the existing rule that range cannot be set for the
  frequency and diode measurement functions.
- Support simulation (`Simulate=1`) for every new attribute and function.
- Mark public functions `_VI_FUNC` and private functions `static`. Do not use unsigned
  types in the public API. Use bounded string operations and check every VISA/IVI status.
- Update `avr6552.fp` and `avr6552.sub` for every new or changed function or attribute,
  documenting valid values, ranges, and defaults, then regenerate the HTML help.
- If the API changes, update the LabVIEW wrapper in `labview/` (`avr6552.rc`, the
  `avr6552_*.mnu` palettes, and `avr6552.llb`) and save it in the oldest supported
  LabVIEW version.
- Preserve backwards compatibility: do not rename functions, attributes, parameters, or
  wrapper VIs, and do not reorder parameter lists or change wrapper connector panes.
- If the model list changes, update `AVR6552_SUPPORTED_INSTRUMENT_MODELS` in
  `avr6552.h`, `config/avr6552.xml`, and `documentation/avr6552.txt`.
- Update the copyright year, `AVR6552_MAJOR_VERSION`/`AVR6552_MINOR_VERSION`, and the
  modification history at the top of `avr6552.c`.
- Run Uncrustify on modified `.c`/`.h` files.

# Testing

Summarize your results in the pull request: instrument model (R6552, R6552T, or
R6552T-R), firmware revision, GPIB interface used, and CVI/LabVIEW versions.

- **Build:** Build `avr6552.prj` for 32-bit and 64-bit with no errors or warnings.
- **Hardware:** Test new and existing functions against a real R6552-series DMM over GPIB.
- **Simulation:** Repeat with `Simulate=1` and confirm new attributes and functions work
  without hardware.
- **Class driver:** Exercise the driver through the IviDmm class API to confirm
  interchangeability, and update the capability table in `documentation/avr6552.txt`.
- **Unit tests:** Run the tests in `test/` and add coverage for new functionality.
- **Example and wrapper:** Build and run `example/avr6552_example.c`, and the LabVIEW
  wrapper if it was modified.
- **Known issues:** Record any unresolved failures in `documentation/avr6552.txt` and the
  README.

# Developer Certificate of Origin (DCO)

   Developer's Certificate of Origin 1.1

   By making a contribution to this project, I certify that:

   (a) The contribution was created in whole or in part by me and I
       have the right to submit it under the open source license
       indicated in the file; or

   (b) The contribution is based upon previous work that, to the best
       of my knowledge, is covered under an appropriate open source
       license and I have the right under that license to submit that
       work with modifications, whether created in whole or in part
       by me, under the same open source license (unless I am
       permitted to submit under a different license), as indicated
       in the file; or

   (c) The contribution was provided directly to me by some other
       person who certified (a), (b) or (c) and I have not modified
       it.

   (d) I understand and agree that this project and the contribution
       are public and that a record of the contribution (including all
       personal information I submit with it, including my sign-off) is
       maintained indefinitely and may be redistributed consistent with
       this project or the open source license(s) involved.

(taken from [developercertificate.org](https://developercertificate.org/))

See [LICENSE](https://github.com/ASingh-NI/instr-driver-advantest-r6552-ivi/blob/main/LICENSE.md) for details about how instr-driver-advantest-r6552-ivi is
licensed.
