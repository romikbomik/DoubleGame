import os
import json
import xml.etree.ElementTree as ET

image_folder = os.path.dirname(__file__)

# Create a list of image file extensions to filter only image files
image_extensions = ['.jpg', '.jpeg', '.png', '.bmp']
annotation_extension = '.xml'
# Create a dictionary to map labels to subfolder paths

config_file = image_folder + "\\"+"config.json"
label_map = {}
with open(config_file, "r") as f:
    label_map = json.load(f)
label_map.sort()
counter = 0
stats = [0] * len(label_map)

for filename in os.listdir(image_folder):
    # Check if the file is an image (based on file extension)
    if filename.lower().endswith(annotation_extension):
        counter += 1
        xml_path = os.path.join(image_folder, filename)
        tree = ET.parse(xml_path)
        root = tree.getroot()

        image_filename = root.find("filename").text
        image_width = int(root.find("size/width").text)
        image_height = int(root.find("size/height").text)
        for obj_elem in root.findall("object"):
            name = obj_elem.find("name")
            if name.text in label_map:
                stats[label_map.index(name.text)] += 1
print(f"total file processed: ${counter}")
for label in label_map:
    label_index = label_map.index(label)
    label_count = stats[label_index]
    print(f"${label} count: {label_count}")
user_input = input("Press Enter to continue...")