import argparse

import numpy as np
from PIL import Image

import decode

def decoder_arguments() -> argparse.Namespace:
    """Parse arguments for decoder

    Returns:
        argparse.namespace: parsed arguments
    """
    parser = argparse.ArgumentParser(
        description="Decode UWOrbital Payload JPEG Compression"
    )
    parser.add_argument(
        "filename", type=str, help="name of binary file to decode"
    )
    parser.add_argument(
        "x_resolution",
        type=int,
        default=1,
        help="number of pixels along horizontal axis"
    )
    parser.add_argument(
        "y_resolution",
        type=int,
        default=1,
        help="number of pixels along vertical axis"
    )
    parser.add_argument(
        "--codes_filename", "-c",
        type=str,
        default="decoder/codes.bin",
        help="name of binary file containing huffman codes"
    )
    parser.add_argument(
        "--greyscale", "-g",
        action='store_const',
        dest='grey',
        const=True,
        default=False,
        help="decodes greyscale image if true"
    )
    args = parser.parse_args()
    return args

args = decoder_arguments()
with open(args.codes_filename, 'rb') as codes_file:
    decode.load_codes(codes_file)
with open(args.filename, 'rb') as data_file:
    data_stream = decode.Bitstream(data_file)
    img_arr = decode.decode(data_stream, (args.x_resolution, args.y_resolution), grey=args.grey)
img_arr = decode.trim_domain(img_arr)
img = Image.fromarray(img_arr.astype(np.uint8))
img.save(f'{args.filename}_decoded.jpg')
img.show()
