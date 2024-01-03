workspace_dir := "/home/vagrant/workspace/cxx/walnut"

# --------------------- projects --------------------------
genesis-build:
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{workspace_dir}}
  xmake -P Genesis 

genesis-watch:
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{workspace_dir}}
  watchexec -w Genesis/src -- "xmake -P Genesis"

genesis: genesis-build
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{workspace_dir}}
  xmake r -P Genesis

raytracing-build:
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{workspace_dir}}
  xmake -P Raytracing 

raytracing-watch:
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{workspace_dir}}
  watchexec -w Raytracing -e h,hpp,cpp -- "xmake -P Raytracing"

raytracing: raytracing-build
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{workspace_dir}}
  xmake r -P Raytracing

sparkai-build:
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{workspace_dir}}
  xmake -P spark

sparkai-run: sparkai-build
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{workspace_dir}}
  xmake r -P spark

sparkai3-build:
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{workspace_dir}}
  xmake -P spark3

sparkai3-run: sparkai3-build
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{workspace_dir}}
  xmake r -P spark3

walnutapp-build:
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{workspace_dir}}
  xmake -P WalnutApp

walnutapp-run: 
  #!/usr/bin/env bash
  set -euxo pipefail
  cd {{workspace_dir}}
  xmake r -P WalnutApp

compile_commands:
  #!/usr/bin/env bash
  cd {{workspace_dir}}
  xmake project -k compile_commands

# vim: set ft=make :
