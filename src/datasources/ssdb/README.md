# SSDB datasource

Open `ssdb://127.0.0.1:43234` (or `127.0.0.1:43234`) in Kst while the
SSDB server is running. Each SSDB scalar **time series** is a Kst vector;
enter the address directly into the datasource field in the New Vector or
Change File dialog (the file-browser button browses local files only).
`localhost` is mapped to `127.0.0.1`: the current Rust consumer tries only
the first resolved address, which may otherwise be IPv6 `::1` even if the
server is listening only on IPv4.
The synthetic Kst vector `INDEX` has one sample per frame, equal to that
frame's number, and can be used as an X axis. Reads extending past the last
frame return only the available frames. The name `INDEX` is reserved and takes
precedence over an SSDB field of the same name.
SSDB matrix streams are available as indexed Kst matrices; SSDB vector streams
are not supported yet. `FRAMES` is a Kst
scalar, and `FILE` is the server address string. Field units, quantity, nominal
rate and samples-per-frame are available as vector metadata.

Frame 0 is the Unix epoch. The datasource configuration dialog sets the frame
rate (default 5 Hz) and an end delay (default 100 ms); both are saved in the
Kst session and per-server settings. Each field's samples per frame is fixed at
connection time to round(nominal field rate / frame rate), at least one. All
fields share the same frame boundaries. The latest frame is the last boundary
not later than the latest timestamp **across all SSDB streams** minus the end
delay. New field registrations become visible on reopening the datasource.

Matrix samples use zero-order hold at the requested frame boundary. A missing
matrix is represented as an empty (0x0) matrix, not a matrix filled with zeros.
Retrieval failures are logged separately. Stream-length queries use the cached
frame count without fetching an epoch-time matrix; navigation does not need a
sample at frame 0. Actual frame-specific dimension and data requests still read
the selected sample.

**Choose a recent starting frame or count from the end.** Reading from epoch
frame 0 to the present would require billions of frames; Kst rejects such a
request before allocation. At 5 Hz, a starting frame for a Unix timestamp in
seconds is approximately `timestamp * 5`.

For historical data, start the **SSDB database server** with
`--read-from-disk`. By default it reads only the most recent data retained in
RAM (about 60 seconds, or roughly 300 frames at 5 Hz), even though older
samples may already be stored on disk. Kst cannot change this server setting;
without it, older requests return zero-valued samples while `INDEX` still
returns valid frame numbers. Restart the database server with the flag to read
older data; restarting Kst alone does not enable historical reads.
If older ranges still contain inconsistent zero gaps with disk reads enabled,
rebuild and restart the database server: older server binaries could over-read
the final disk chunk in a time-window request and silently return zeros for
the whole window. The Kst plugin alone cannot repair that server-side read.

## Limitations of the current C API

The API returns resampled values without raw sample timestamps. It currently
performs zero-order hold for *all* scalar types, including floating point. It
cannot reliably distinguish a missing sample before the first measurement from
a genuine zero, so NaN-before-first and linear interpolation are not yet
possible. After a field's last reported sample, the datasource attempts to
hold that value. The C API also does not report the actual length or error
status of returned scalar arrays, so empty intervals and transport failures
cannot always be distinguished. Correct boundary behavior and float-linear
interpolation require a future timestamped-sample API with explicit status
and memory-management functions.

To build the optional plugin, configure Kst with `SSDB_WORKSPACE_ROOT` set to
the cmd-tlm workspace containing the `telemetry_c` crate. By default Kst
looks for a sibling `ss/cmd-tlm` workspace. The build uses Cargo and the
corresponding generated C header from that workspace.
The linked `telemetry_c` must return typed scalar tokens from
`consumer_register_scalar`; older builds boxed a generic token instead,
causing scalar reads to return zeros. Rebuild the plugin when updating the
Rust library and restart Kst to load the new module.

Matrix reads also require the hardened `consumer_get_selected_matrix`: successful
reads return exactly the requested number of valid entries (including empty
matrices for missing samples), while failures return NULL. Rebuild Kst and all
datasource plugins together after updating: the datasource interface key is now
2.1, and older binary plugins are not compatible with the new frame-count method.
