#!/bin/bash

export PYTHONPATH=${PYTHONPATH}:`pwd`/src:`pwd`/test
echo "PYTHONPATH set to $PYTHONPATH"

function parse_git_branch {
    # show branch in shell prompt
   ref=$(git symbolic-ref HEAD 2> /dev/null) || return
   BRANCH=""${ref#refs/heads/}""
   if [ "$BRANCH" == "master" ]; then
       echo "($(tput setaf 1)${BRANCH}$(tput sgr0))";
   elif [ "$BRANCH" == "develop" ]; then
       echo "($(tput setaf 3)${BRANCH}$(tput sgr0))";
   elif [[ "$BRANCH" == "feature"* ]]; then
        echo "($(tput setaf 2)${BRANCH}$(tput sgr0))";
   elif [[ "$BRANCH" == "release"* ]]; then
       echo "($(tput setaf 5)${BRANCH}$(tput sgr0))";
   else
       echo  "(${BRANCH})"
   fi

}
export PS1="[\t \W]\$(parse_git_branch) "

function ns_pip(){
    # install a package from the NS pypi repo
    # usage: ns_pip <package> <version>
    local PACKAGE=$1
    local VERSION=$2
    if [ "x$PACKAGE" == "x" ]; then
        echo "usage: ns_pip <package> <version>";
        return 1;
    fi
    if [ "x$VERSION" == "x" ]; then
        echo "usage: ns_pip <package> <version>";
        return 1;
    fi
    TOKEN=`git config --get pypi.user`
    KEY=`git config --get pypi.key`


    pip install -i https://${TOKEN}:${KEY}@repo.n-s.us/simple ${PACKAGE}==${VERSION}
}

function ns_pip_update(){
    # update an NS package to the latest version
    # usage: ns_pip_update <packagename>
    local PACKAGE=$1
    TOKEN=`git config --get pypi.user`
    KEY=`git config --get pypi.key`
    pip install -i https://${TOKEN}:${KEY}@repo.n-s.us/simple --upgrade ${PACKAGE}
}
