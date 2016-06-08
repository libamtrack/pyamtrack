import argparse
import os
import subprocess
import pyamtrack


def get_momentum(E_MeV_u):
    return pyamtrack.pyamtrack.AT_momentum_from_E_MeV_c_u_single(E_MeV_u)


def main():
    """
    Main function, called from CLI script
    :return:
    """
    import pyamtrack
    parser = argparse.ArgumentParser()
    parser.add_argument('--verbose',
                        action='store_true',
                        help='be verbose')
    parser.add_argument('--version',
                        action='version',
                        version=pyamtrack.__version__)
    args = parser.parse_args()

    momentum = get_momentum(10.0)

    print("Momentum = ", momentum)


if __name__ == '__main__':
    main()
