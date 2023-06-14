git submodule status | awk '{print $2}' | while read -r line; do
    mkdir -p backup/$(dirname $line)
    cp -R "$line" "backup/$line"
    git submodule deinit --force "$line"
    git rm -r --cached "$line"
    mkdir -p $line
    cp -R backup/$line/* "$line"
    rm -rf "$line/.git"
done
