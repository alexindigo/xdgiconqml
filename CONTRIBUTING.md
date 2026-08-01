# Contributing to xdgiconqml

## Polling and periodic timers

Any `QTimer` running on a repeating interval below 30 seconds inside a
library component (as opposed to a user-visible animation) must include:

1. A comment justifying the interval (why not longer, what breaks if it
   is shorter).
2. A reference to the specific failure mode the polling recovers from
   (which filesystem, which race, etc).
3. A note on wakeup cost, especially for battery-powered devices.

**Rationale:** xdgiconqml runs inside long-lived shell processes. A
5-second timer costs approximately 17 000 unnecessary wakeups per day
on an idle system. The default approach should be event-driven
(`QFileSystemWatcher`); polling is the fallback.
