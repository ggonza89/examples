#!/usr/bin/env python
"""
    this is the source code for a brute force application.

    Evaluates any two numbers in a list adding to a value given.

"""
class BfException(Exception):
        """
        BfException is raised whenever a non-float input or an empty list is
        encountered.

        The output of BfException is the string provided when raised.

        """

        def __str__(self):
            return ': '.join(self.args)

def bruteforce (list_num, value, evalexpress):
    """
    brute force runs in O(n^2) to check for any two numbers in list_num
    that add to value

    the argument list_num is just a list of floats and value is just a
    float that is checked against every addition.

    the argument evalexpress is the evaluation that the user wants done on
    the list, in this case we want the
    evaluation to be of two floats being equal and close to epsilon,
    epsilon being chosen by user.

    brute force returns a list of tuples that evaluate to value.

    if brute force ever encounters an input that is not a float it will
    raise a BfException.  See docstring of BfException.

    """

    answers = []

    if len(list_num) == 0:
        raise BfException("The list of numbers is empty")

    for i in range(len(list_num)):
        for k in range(len(list_num)):
            try:
                if evalexpress(list_num[i], list_num[k], value):
                    answers.append((list_num[i], list_num[k]))
            except TypeError:
                raise BfException("Only float values are accepted...")
            except ValueError:
                raise BfException("Cannot evaluate to a character...")

    return answers

def dictalg(list_num, value, evalexpress):
    """
    this algorithm checks for any two numbers in list that sum to the value
    given by using a dictionary and runs in O(n)

    the argument list_num is just a list of floats and value is just a
    float that is checked against every addition.

    the argument evalexpress is the evaluation that the user wants done on
    the list, in this case we want the
    evaluation to be of two floats being equal and close to epsilon,
    epsilon being chosen by user.

    returns a list of tuples that evaluate to value.

    if ever encounters an input that is not a float it will
    raise a BfException.  See docstring of BfException.

    """

    answers = []

    try:
        value = float(value)
    except ValueError:
        raise BfException("Only float values are accepted...")


    if len(list_num) == 0:
        raise BfException("The list of numbers is empty")

    list_dict = {}

    for i in range(len(list_num)):
        try:
            if list_dict.has_key(value - float(list_num[i])):
                if evalexpress(value-list_num[i], list_num[i], value):
                    answers.append((value-list_num[i]list_num[i]))

            list_dict[value-list_num[i]] = list_num[i]
        except TypeError:
            raise BfException("Only float values are accepted...")
        except ValueError:
            raise BfException("Cannot evaluate to a character...")

    return answers

