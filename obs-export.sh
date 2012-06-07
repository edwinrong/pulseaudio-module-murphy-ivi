#!/bin/bash

# This needs a bit more work, mostly on the "discplined engineering" front.
# IOW, instead of this UPSTREAM_BASE hack it would be better to have 3
# branches:
#   1) pristine upstream: for tracking upstream progress/retrogression
#   2) patched upstream: pristine upstream with outr patches applied
#   3) working local: patches upstream + a set of scripts (like this) to
#      do everyday stuff like making new releases, exporting stuff to
#      OBS, etc...


PKG="$(basename `pwd` | tr [:upper:] [:lower:])"
UPSTREAM_BASE="HEAD"
VERSION="`date +'%Y%m%d'`"


while [ "${1#-}" != "$1" -a -n "$1" ]; do
    case $1 in
        --name|-n)
            PKG="$2"
            shift 2
            ;;
        --version|-v)
            VERSION="$2"
            shift 2
	    ;;
        --base|-b)
            UPSTREAM_BASE="$2"
            shift 2
            ;;
        --help|-h)
           echo "usage: $0 [-n <name>][-v <version>][-b <upstream-base>]"
           echo ""
           echo "<name> is the name of the package, <version> is the version"
           echo "you want to export to OBS, and <upstream-base> is the name of"
           echo "the upstream git branch or the SHA1 you want to generate your"
           echo "OBS tarball from and base your patches on top of. The output"
           echo "will be generated in a directory called obs-$VERSION."
	   echo ""
           echo "E.g.:"
           echo "  $0 -n $PKG -v 2.0 -b $PKG-upstream"
           echo ""
           echo "This will produce an OBS export with version 2.0 against the"
           echo "SHA1 $PKG-2.0 and place the result in obs-2.0."
           exit 0
           ;;
        *) echo "usage: $0 [-n <name>][-v <version>][-b <upstream-base>]"
           exit 1
           ;;
    esac
done

echo "PKG=$PKG"
echo "VERSION=$VERSION"
echo "UPSTREAM_BASE=$UPSTREAM_BASE"

rm -fr obs-$VERSION
mkdir obs-$VERSION && \
    git archive --format=tar --prefix=$PKG-$VERSION/ $UPSTREAM_BASE \
        > obs-$VERSION/$PKG-$VERSION.tar && \
    gzip obs-$VERSION/$PKG-$VERSION.tar && \
cd obs-$VERSION && \
    git format-patch -n $UPSTREAM_BASE && \
    cat ../$PKG.spec.in | sed "s/@VERSION@/$VERSION/g" > $PKG.spec.in && \
cd -

cd obs-$VERSION
patchlist="`ls *.patch 2>/dev/null`"
[ "$patchlist" = "*.patch" ] && patchlist=""
cat $PKG.spec.in | while read line; do
    case $line in
        @DECLARE_PATCHES@)
            i=0
            for patch in $patchlist; do
                echo "Patch$i: $patch"
                let i=$i+1
            done
            ;;
        @APPLY_PATCHES@)
            i=0
            for patch in $patchlist; do
                echo "%patch$i -p1"
                let i=$i+1
            done
            ;;
        *)
            echo "$line"
            ;;
    esac
done > $PKG.spec
cd -
rm -f obs-$VERSION/$PKG.spec.in
