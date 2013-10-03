#!/usr/bin/env python
"""
unittesting for the brute force algorithm that takes in a list of numbers
and checks them against the value given.

"""
import unittest
import sys


from intern_alg import bruteforce, dictalg, BfException

class TestAlg(unittest.TestCase):
    """
    runs unittest on the brute force code.

    """
    def setUp(self):
        """
        initialize any variables needed for unittest.TestCase

        """

        # evaluate equality of floats to the precision 0.0001
        self.evalexpress = lambda a, b, val: abs((a+b)-val) <= 0.0001

    def test_data_samples(self):
        """
        the first unittest tests data samples to return
        the proper list of numbers that add to the value.

        """

        #test the code on an empty array
        self.assertRaises(BfException, bruteforce, [], 0, self.evalexpress)
        self.assertRaises(BfException, dictalg, [], 0)

        #test code on an unattainable value
        self.assertEqual(bruteforce([0, 0], 1, self.evalexpress), [])
        self.assertEqual(dictalg([0, 0], 1), [])

        #test code on attaining a negative value
        self.assertEqual(bruteforce([-1, -2, 3, 4], -3, self.evalexpress),
            [(-1, -2), (-2, -1)])
        self.assertEqual(dictalg([-1, -2, 3, 4], -3),
            [(-2.0, -1.0)])

        #test code on float values to attain something close to epsilon
        self.assertEqual(bruteforce([1.001, -1.00, 3.45, 3.5678], 0.001,
            self.evalexpress), [(1.001, -1.00), (-1.00, 1.001)])
        self.assertEqual(dictalg([1.001, -1.00, 3.45, 3.5678], 0.001),
            [(-1.00, 1.001)])

        #test code on non-leading zero floats
        self.assertEqual(bruteforce([-.121, .121, 5.4, 9, 6], 0,
            self.evalexpress), [(-.121, .121), (.121, -.121)])
        self.assertEqual(dictalg([-.121, .121, 5.4, 9, 6], 0), [(.121, -.121)])

        #test code for epsilon in evalexpress
        self.assertEqual(bruteforce([4, 1.0001, 1, 2, 4, 0], 2.0000,
            self.evalexpress), [(1.0001, 1), (1, 1.0001),
            (1, 1), (2, 0), (0, 2)])
        self.assertEqual(dictalg([4, 1.0001, 1, 2, 4, .0001], 2.0001), [(0.0001
            , 2)])

    def test_failure(self):
        """
        the second unittest causes the method, bruteForce, to fail if given
        wrong input such as a character when only floats are allowed.

        """

        #test code on a char as value
        self.assertRaises(BfException, bruteforce, [0, 1, 2, 3, 4, 5], 'b',
            self.evalexpress)
        self.assertRaises(BfException, dictalg, [0, 1, 2, 3, 4, 5], 'b')

        #test code on a char in list
        self.assertRaises(BfException, bruteforce, [4, 1, 0, 5, 8, 4, 'a'], 5,
                          self.evalexpress)
        self.assertRaises(BfException, dictalg, [4, 1, 0, 5, 8, 4, 'a'], 5)

if __name__ == '__main__':
    unittest.main()
