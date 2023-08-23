workspace_dir := "/home/vagrant/workspace"
genesis_dir := workspace_dir / "cxx/walnut/Genesis"
raytracing_dir := workspace_dir / "cxx/walnut/Raytracing"
walnutapp_dir := workspace_dir / "cxx/walnut/WalnutApp"

# --------------------- projects --------------------------
genesis-build:
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{genesis_dir}}
  xmake -P .

genesis-watch:
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{genesis_dir}}
  watchexec -w src -- "xmake -P ."

genesis: genesis-build
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{genesis_dir}}
  xmake r Genesis

raytracing-build:
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{raytracing_dir}}
  xmake -P . 

raytracing-watch:
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{raytracing_dir}}
  watchexec -e h,hpp,cpp -- "xmake -P ."

raytracing: raytracing-build
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{raytracing_dir}}
  ./Raytracing

walnutapp-build:
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{walnutapp_dir}}
  xmake -P . 

walnutapp: walnutapp-build
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{walnutapp_dir}}
  ./WalnutApp

build:
  #!/usr/bin/env bash
  cd {{walnutapp_dir}}
  xmake project -k compile_commands

# vim: set ft=make :
