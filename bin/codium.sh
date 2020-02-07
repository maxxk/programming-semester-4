#!/bin/sh
VERSION="1.41.1"

TMPDIR="$XDG_RUNTIME_DIR"
DIR="$TMPDIR/codium"

CODIUM="$DIR/bin/codium --extensions-dir=$DIR/extensions --user-data-dir=$DIR/data"

function installCodium() {
    mkdir -p ~/.config/VSCodium-portable/
    touch ~/.config/VSCodium-portable/{keybindins.json,settings.json}
    mkdir -p "$DIR"


    echo "In directory: "
    cd "$TMPDIR/codium"
    echo "Downloading"
    curl -O -L https://istina.msu.ru/media/common/VSCodium-linux-x64-$VERSION.tar.gz
    echo "Unpacking"
    tar xf VSCodium-linux-x64-$VERSION.tar.gz

    mkdir -p extensions
    mkdir -p data/user-data/User/
    
    # Save preferences
    ln -s ~/.config/VSCodium-portable/keybindins.json data/user-data/User/
    ln -s ~/.config/VSCodium-portable/settings.json data/user-data/User/

    $CODIUM --force --install-extension eamodio.gitlens
    $CODIUM --force --install-extension ms-vscode.cpptools
}

if [ -f $DIR/codium ]; then
    $CODIUM "$@"
else
    installCodium
    $CODIUM "$@"
fi

