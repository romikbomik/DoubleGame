import os
import json
import xml.etree.ElementTree as ET

image_folder = os.path.dirname(__file__)
target_objet_per_image = 8

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
        counter = 0
        image_filename = root.find("filename").text
        for obj_elem in root.findall("object"):
            counter += 1
        if (counter < target_objet_per_image):
            print(f"File: {filename} has: {counter} objects")
