import tempfile
from pathlib import Path
import unittest
import zipfile

from tools.zip_directory import create_zip


class ZipDirectoryTests(unittest.TestCase):
    def test_audio_pack_preserves_names_bytes_and_stored_compression(self):
        with tempfile.TemporaryDirectory(prefix="castle archive ") as directory:
            source = Path(directory)
            (source / "voice.wav").write_bytes(b"RIFF\x00\xffWAVE")
            (source / ".svn").mkdir()
            (source / ".svn" / "entries").write_text("metadata")
            archive = source / "audio.pack"
            archive.write_bytes(b"old archive")
            create_zip(archive, source, stored=True)
            with zipfile.ZipFile(archive) as result:
                self.assertEqual(result.namelist(), ["voice.wav"])
                self.assertEqual(result.read("voice.wav"), b"RIFF\x00\xffWAVE")
                self.assertEqual(result.getinfo("voice.wav").compress_type, zipfile.ZIP_STORED)
            self.assertFalse(source.joinpath("audio.pack.tmp").exists())

    def test_symbol_archive_keeps_bundle_directory(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "Castle.app.dSYM" / "Contents"
            source.mkdir(parents=True)
            (source / "symbols").write_bytes(b"debug symbols")
            archive = root / "symbols.zip"
            create_zip(archive, source.parent, include_root=True)
            with zipfile.ZipFile(archive) as result:
                self.assertEqual(result.namelist(), ["Castle.app.dSYM/Contents/symbols"])
                self.assertEqual(result.testzip(), None)


if __name__ == "__main__":
    unittest.main()
