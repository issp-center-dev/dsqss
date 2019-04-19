# This is not for tag, finish
if [ -z "$TRAVIS_TAG" ]; then exit 0; fi

DIRNAME=DSQSS-${TRAVIS_TAG}
wget https://github.com/${TRAVIS_REPO_SLUG}/archive/${TRAVIS_TAG}.tar.gz -O tarball.tar.gz
mkdir $DIRNAME
tar -xz -C $DIRNAME --strip-components=1 -f tarball.tar.gz
cp -r build/doc/jp/pdf/DSQSS.pdf ${DIRNAME}/DSQSS-jp-${TRAVIS_TAG}.pdf
cp -r build/doc/en/pdf/DSQSS.pdf ${DIRNAME}/DSQSS-en-${TRAVIS_TAG}.pdf
tar czf ${DIRNAME}.tar.gz ${DIRNAME}
rm tarball.tar.gz
