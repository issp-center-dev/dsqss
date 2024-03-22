import site
import sys
import pathlib

if len(sys.argv) < 2:
    print("Usage: get_site_packages.py <prefix>")
    sys.exit(1)

for spdir in site.getsitepackages([sys.argv[1]]):
    p = pathlib.Path(spdir)
    if p.exists():
        print(p.resolve())
        break
