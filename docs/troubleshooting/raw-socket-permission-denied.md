# Raw Socket Permission Denied

> **Status:** Draft — placeholder content. Final technical prose is forthcoming.


Managing CAP_NET_RAW and root execution boundaries.

## Grant

File capabilities beat sudo for daily runs.

## Scope

Only the capture binary needs the capability.

```bash
$ sudo setcap cap_net_raw+ep ./build/sentinel_lab
$ getcap ./build/sentinel_lab
```

---

*Part of the sentinel-lab documentation set. See mkdocs.yml for navigation.*
