"""Package the Blender File > Export add-on without installing user preferences."""
from pathlib import Path
import zipfile

folder = Path(__file__).resolve().parent
output = folder.parents[1]/'build/packages/castle_bdae_tools.zip'
output.parent.mkdir(parents=True, exist_ok=True)
with zipfile.ZipFile(output, 'w', zipfile.ZIP_DEFLATED) as archive:
    for name in ['__init__.py', 'export_bdae.py', 'import_bdae.py', 'bdae_reader.py', 'bdae_writer.py', 'bdae_scene.py', 'scene_tools.py', 'README.md']:
        archive.write(folder/name, 'castle_bdae_tools/'+name)
print(output)
