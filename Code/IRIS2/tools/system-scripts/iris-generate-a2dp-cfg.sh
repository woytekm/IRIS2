#!/bin/ash

cat <<EOL
pcm.!default {
    type plug
    slave {
        pcm {
            type bluetooth
EOL
echo "            device $1"
cat <<EOL
            profile "auto"
        }
    }
    hint {
        show on
        description "BT speaker"
    }
}
ctl.!default {
  type bluetooth
}
EOL

