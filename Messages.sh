#! /bin/bash
# Adapted from the Messages.sh file in trojita
# xgettext-generated .po files use different context than QObject::tr.
# The generated files use something like a file name while QObject::tr expects
# class names. This approach works.
 
rm -f "${podir}/kst.ts"
lupdate -silent -recursive src/ -ts "${podir}/kst.ts"
lconvert "${podir}/kst.ts" --sort-contexts --output-format pot -o "${podir}/kst_common.pot"
rm "${podir}/kst.ts"
