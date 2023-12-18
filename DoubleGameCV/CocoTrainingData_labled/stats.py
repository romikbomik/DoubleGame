import os
import json
import cv2
import xml.etree.ElementTree as ET
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image


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


def show_items(label):
    found = []
    names = []
    for filename in os.listdir(image_folder):
        # Check if the file is an image (based on file extension)
        if filename.lower().endswith(annotation_extension):
            xml_path = os.path.join(image_folder, filename)
            img_filename = filename[:-4] + '.jpg'
            img_file_path = os.path.join(image_folder, img_filename)
            img = cv2.imread(img_file_path)
            img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

            tree = ET.parse(xml_path)
            root = tree.getroot()
            objects_counter = 0
            image_filename = root.find("filename").text
            image_width = int(root.find("size/width").text)
            image_height = int(root.find("size/height").text)
            for obj_elem in root.findall("object"):
                name = obj_elem.find("name")
                xmin = int(obj_elem.find('bndbox').find('xmin').text)
                xmax = int(obj_elem.find('bndbox').find('xmax').text)
                
                ymin = int(obj_elem.find('bndbox').find('ymin').text)
                ymax = int(obj_elem.find('bndbox').find('ymax').text)
                if(name.text == label):
                    roi = img_rgb[ymin:ymax, xmin:xmax]
                    found.append(roi)
                    names.append(img_filename)
    max_height = max(roi.shape[0] for roi in found)

    # Resize all ROIs to have the same height (you can adjust the width as needed)
    resized_rois = [cv2.resize(roi, (int(roi.shape[1] * max_height / roi.shape[0]), max_height)) for roi in found]

    for name in names:
        print(name)
    # Combine resized ROIs side by side
    combined_image = np.concatenate(resized_rois, axis=1)
    # Display the combined image
    fig, a = plt.subplots(1,1)
    fig.set_size_inches(5,5)
    a.imshow(Image.fromarray(combined_image))
    a.set_axis_off()
    plt.show()
                
for filename in os.listdir(image_folder):
    # Check if the file is an image (based on file extension)
    if filename.lower().endswith(annotation_extension):
        counter += 1
        xml_path = os.path.join(image_folder, filename)
        img_filename = filename[:-4] + '.jpg'
        img_file_path = os.path.join(image_folder, img_filename)
        img = cv2.imread(img_file_path)
        img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

        tree = ET.parse(xml_path)
        root = tree.getroot()
        objects_counter = 0
        image_filename = root.find("filename").text
        image_width = int(root.find("size/width").text)
        image_height = int(root.find("size/height").text)
        for obj_elem in root.findall("object"):
            name = obj_elem.find("name")
            objects_counter += 1
            if name.text in label_map:
                stats[label_map.index(name.text)] += 1
            else:
                print(f"File: {filename} has invalid label")
        if (objects_counter != target_objet_per_image):
            print(f"File: {filename} has: {objects_counter} objects")
print(f"total file processed: ${counter}")
for label in label_map:
    label_index = label_map.index(label)
    label_count = stats[label_index]
    print(f"{label_index}) {label} count: {label_count}")

user_input = input("Input index...")
user_input = int(user_input)
while(user_input < len(label_map)):
    show_items(label_map[user_input])
    user_input+=1