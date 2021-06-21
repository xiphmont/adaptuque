# adaptuque
The Adaptuque: A Warm Little Hat for your stock Adaptec Expander Firmware

# What is it?

Adaptuque is an in-progress patch series for 12Gbps SAS3 expanders
based on Adaptec's reference expander designs.  These cards currently
include the AEC-83600 and AEC-82885T sold by Adaptec/Microchip, IBM,
Lenovo and others, as well as the nearly identical Intel RES3TV360 and
RES3FV288.  HP's SAS3 cards are also similar, but different enough
that they're not currently supported.

# Is this firmware?

The patch set is not standalone firmware; it must be applied to the
stock firmware as downloaded from the Adaptec or Intel download sites.
This extra little dance is necessary to avoid distributing any
Microchip-copyrighted software. Sorry. The goal here is to make a
really good piece of kit even better, not take a shot at
Adaptec/Microchip, so I hope you understand.  Hopefully someday
Adaptec will grant permission to just offer firmware directly, but
they have not.

# What does this patch do?

1) Fully configurable SGPIO output via a simple persistent
configuration utility.  Take complete control of backplane indication
via the SES (SCSI Enclosure Services) state available in the expander.

2) Configurable port assignment and mapping, supporting up to 32 (yes,
a full 32!) direct attached drives per expander.  This includes drives
hung off passive backplanes.

3) Bugfixes.  No more expander hangs, crashes, or garbled output on
several common SES requests, etc.

4) Etc, as the need arises.

# Is this some sort of crack?

tldr, No; this patch does not enable or expose any features that
aren't already supported in some way by the stock cards, and you
obviously can only use it on a card you've bought, using firmware you
already have. I have the individual right to modify cards I own for my
personal use, and so do you.

That said, it _is_ reverse-engineed from firmware pulled off of
second-hand cards, and cross-referenced against firmware binaries made
available for download by Adaptec and Intel.  The clickwrap EULA
expressly forbids reverse engineering, which I have done.
Technically, that is a breach of civil contract.  However, I have no
interest in harming Adaptec's trade secrets, copyright, or
intellectual property.  The patch isn't looking to pick any fights,
just improve the firmware.

The patch itself is original code that replaces segments of the stock
firmware.  It does not include any of the original firmware itself. It
has to be applied to a firmware package downloaded from an official
source.  When applied, the patch creates a normal firmware update that
fixes bugs, streamlines a little nastiness, and adds some
configurability. That's all. It flashes with Adaptec's standard
utilities, makes no permanent changes, and the card can be reverted to
stock firmware at any time.

Perhaps Adaptec will be fine with all this; I'd love to be able to
upload the extensive Ghidra files involved in writing these patches.
But for now, that's a no-go.

# How do I use it?

More to come, this is currently mostly a placeholder.