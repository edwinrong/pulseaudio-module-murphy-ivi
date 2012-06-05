dnl Initiate the documentation directories
dnl
dnl PA_DOCINIT([depdir],[doxyfilename],[docdir])
dnl

AC_DEFUN([PA_DOCINIT],
[
  m4_ifset([$1], [depdir=$1], [depdir=.deps]) 
  m4_ifset([$2], [doxyfile=$1], [doxyfile=Doxyfile])
  m4_ifset([$3], [docdir=$2], [docdir=doc])

  AC_PATH_TOOL( [PA_FIND], find )
  AC_PROG_SED
  AC_PROG_MKDIR_P

  AS_IF( [ test "x$PA_FIND" = "x" -o "x$MKDIR_P" = "x" ],
         [ AC_MSG_ERROR([essential programs are missing to init docs]) ],
         [ found=`$PA_FIND $docdir -name $doxyfile`
           for f in "$found" ; do
               $MKDIR_P `echo $f | $SED -e "s/$doxyfile//"`$depdir
           done])
])
