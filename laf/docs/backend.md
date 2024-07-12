# laf backend

*laf* can be compiled with different backends. A backend is another
library or platform used to implement *laf* functionality/API.

* `LAF_BACKEND=skia`: `laf-os` will use Skia for drawing on the native window
* `LAF_BACKEND=none`: Mainly for CLI apps (when no UI is required)
