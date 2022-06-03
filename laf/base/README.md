# LAF Base Library
*Copyright (C) 2001-2017 David Capello*

> Distributed under [MIT license](../LICENSE.txt)

Cross-platform core functionality to do basic tasks:

* Data utilities ([encode/decode_base64](base64.h))
* File system & filename/path utilities ([fs.h](fs.h))
* File utilities ([serialization](serialization.h), [sha1](sha1.h), [launcher](launcher.h))
* Logging functions ([LOG()](log.h))
* Manage DLLs ([load/unload_dll()](dll.h))
* Multi-threading ([thread](thread.h), [mutex](mutex.h), [ScopedLock](scoped_lock.h))
* Smart pointers ([SharedPtr](shared_ptr.h))
* String/UTF8 utilities ([string](string.h), [split_string](split_string.h), [trim_string](trim_string.h))
* Timing ([Chrono](chrono.h))
* Type conversion ([convert_to](convert_to.h))
* Unicode filenames ([open_file_raw()](file_handle.h), [FSTREAM_PATH()](fstream_path.h))
* Version comparison ([Version](version.h))
