import logging
import os
import unittest

from unittest import mock

from decoder import decode

# logging.basicConfig(level=logging.DEBUG)

class FileSystemTest(unittest.TestCase):
    TEMP_DIR_PATH = "tmp"
    _obj_cnt = 0
    
    @staticmethod
    def _get_obj_cnt():
        return FileSystemTest._obj_cnt
    
    @staticmethod
    def _add_obj():
        FileSystemTest._obj_cnt += 1

    @staticmethod
    def _remove_obj():
        FileSystemTest._obj_cnt -= 1

    def __init__(self, *args, **kwargs):
        super(FileSystemTest, self).__init__(*args, **kwargs)
        self._add_obj()
        if self._get_obj_cnt() == 1:
            assert not os.path.isdir(self.TEMP_DIR_PATH), f"temp directory '{self.TEMP_DIR_PATH}' already exists"
            os.mkdir(self.TEMP_DIR_PATH)
            logging.debug(f'temp directory created: {os.path.abspath(self.TEMP_DIR_PATH)}')

    def __del__(self):
        self._remove_obj()
        if self._get_obj_cnt() == 0:
            os.rmdir(self.TEMP_DIR_PATH)
            logging.debug('temp directory removed')

class BinFileTest(FileSystemTest):
    MOCK_BIN_FILENAME = None
    MOCK_BITS = None

    def __init__(self, *args, **kwargs):
        super(BinFileTest, self).__init__(*args, **kwargs)
        self.file_path = os.path.join(
            self.TEMP_DIR_PATH, self.MOCK_BIN_FILENAME)

    def setUp(self):
        with open(self.file_path, 'wb') as mock_bin_file:
            mock_bin_file.write(self.MOCK_BITS)

    def tearDown(self):
        os.remove(self.file_path)
    

class TestBitstream(BinFileTest):
    MOCK_BIN_FILENAME = 'mockbin.bin'
    MOCK_BITS = b'\x19\xf7\xea\x27' # 0001 1001, 1111 0111, 1110 1010, 0010 0111

    def test_read_one_byte(self):
        with open(self.file_path, 'rb') as bfile:
            stream = decode.Bitstream(bfile)
            self.assertEqual(stream.read(), '0')
            self.assertEqual(stream.empty(), False)
            self.assertEqual(stream.read(3), '001')
            self.assertEqual(stream.empty(), False)
            self.assertEqual(stream.read(4), '1001')
            self.assertEqual(stream.empty(), False)
            self.assertEqual(stream.read(2), '11')
            self.assertEqual(stream.empty(), False)
            self.assertEqual(stream.read(7), '1101111')
            self.assertEqual(stream.empty(), False)
            
    def test_read_all_bits(self):
        with open(self.file_path, 'rb') as bfile:
            stream = decode.Bitstream(bfile)
            self.assertEqual(
                stream.read(32),
                '00011001111101111110101000100111'
            )
            self.assertEqual(stream.empty(), True)
            
    def test_read_extra_bits(self):
        with open(self.file_path, 'rb') as bfile:
            stream = decode.Bitstream(bfile)
            self.assertEqual(
                stream.read(33),
                '00011001111101111110101000100111'
            )
            self.assertEqual(stream.empty(), True)
            
    def test_twos_complement(self):
        self.assertEqual(decode.Bitstream.twos_complement('0100101'), -90)
        self.assertEqual(decode.Bitstream.twos_complement('1011010'), 90)
