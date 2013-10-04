examples
========

This is full of my first "projects" at narrative science

first run setup.sh

>. setup.sh

this adds the paths to src and test to look in those directories for any
python files

Never develop in master branch
checkout develop first then make a branch of your own feature

>git checkout develop
>git branch -b myfeature
>git checkout myfeature

where myfeature is the name of the feature you want to make

When you are ready to commit any changes to develop add the files then commit

>git add <thefile>
>git commit -m "description of the change"

After commiting you can push the change onto the repo

>git push

To merge these changes onto develop enter

>git checkout develop
>git merge --no-ff myfeature

To remove any branch from the repository

>git branch -d myfeature

Projects
========

This is a description of my projects

    src/intern_alg.py
    =============

    This is the source code that runs two algorithms that are given a list and
    a value.
    The first algorithm runs it in O(n^2) and the second uses a dictionary(
        associative array) to run it in O(n).

    test/test_alg.py
    ===========

    This is the unittest for intern_alg.py.
    Does a little benchmarking to show the difference in speed if it is not
    apparent by reading the code.
