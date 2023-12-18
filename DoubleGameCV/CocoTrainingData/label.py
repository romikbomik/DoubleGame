import cv2
import os
import tkinter as tk
from tkinter import filedialog, ttk
from PIL import Image, ImageTk
import threading
import json
import xml.etree.ElementTree as ET


def createnewObject(name, xmin, ymin, xmax, ymax):
    # Create a new <object> element
    new_object = ET.Element("object")

    # Add child elements to the new object
    name_elem = ET.Element("name")
    name_elem.text = name
    new_object.append(name_elem)

    pose_elem = ET.Element("pose")
    pose_elem.text = "Unspecified"
    new_object.append(pose_elem)

    truncated_elem = ET.Element("truncated")
    truncated_elem.text = "0"
    new_object.append(truncated_elem)

    difficult_elem = ET.Element("difficult")
    difficult_elem.text = "0"
    new_object.append(difficult_elem)

    bndbox_elem = ET.Element("bndbox")
    xmin_elem = ET.Element("xmin")
    xmin_elem.text = str(xmin)
    bndbox_elem.append(xmin_elem)

    ymin_elem = ET.Element("ymin")
    ymin_elem.text = str(ymin)
    bndbox_elem.append(ymin_elem)

    xmax_elem = ET.Element("xmax")
    xmax_elem.text = str(xmax)
    bndbox_elem.append(xmax_elem)

    ymax_elem = ET.Element("ymax")
    ymax_elem.text = str(ymax)
    bndbox_elem.append(ymax_elem)

    new_object.append(bndbox_elem)

    return new_object

user_selection_event = threading.Event()
user_continue_event = threading.Event()
# Set the path to the folder containing your images
image_folder = os.path.dirname(__file__)

# Create a list of image file extensions to filter only image files
image_extensions = ['.jpg', '.jpeg', '.png', '.bmp']
annotation_extension = '.xml'
# Create a dictionary to map labels to subfolder paths

config_file = image_folder + "\\"+"config.json"
with open(config_file, "r") as f:
    label_map = json.load(f)
label_map.sort()
remove_input = "Remove"


# Create subfolders for each label if they don't already exist
#for folder in label_map:
#    os.makedirs(image_folder+".\\" + folder, exist_ok=True)

xml_path = ""
image = {}
image_item = {}

# Create a Tkinter window
window = tk.Tk()
window.title("Image Labeling Tool")

# Create a canvas for displaying images
canvas = tk.Canvas(window, width=512, height=256)
canvas.grid(row=0, column=0, padx=10, pady=10, sticky='nsew')



# Create a Label widget for displaying text
label_xmin = tk.Label(window, text="xmin: ")
label_xmin.grid(row=0, column=0, padx=70, pady=40, sticky='ne')
entry_xmin = tk.Entry(window, width=10)
entry_xmin.grid(row=0, column=0, padx=10, pady=40, sticky='ne')
entry_xmin.insert(0, "0")

label_ymin = tk.Label(window, text="ymin: ")
label_ymin.grid(row=0, column=0, padx=70, pady=70, sticky='ne')
entry_ymin = tk.Entry(window, width=10)
entry_ymin.grid(row=0, column=0, padx=10, pady=70, sticky='ne')
entry_ymin.insert(0, "0")

label_xmax = tk.Label(window, text="xmax: ")
label_xmax.grid(row=0, column=0, padx=70, pady=100, sticky='ne')
entry_xmax = tk.Entry(window, width=10)
entry_xmax.grid(row=0, column=0, padx=10, pady=100, sticky='ne')
entry_xmax.insert(0, "0")

label_ymax = tk.Label(window, text="ymax: ")
label_ymax.grid(row=0, column=0, padx=70, pady=130, sticky='ne')
entry_ymax = tk.Entry(window, width=10)
entry_ymax.grid(row=0, column=0, padx=10, pady=130, sticky='ne')
entry_ymax.insert(0, "0")

def on_entry_focus_out(event):
    try:
        # Get the integer value from the Entry
        xmin = int(entry_xmin.get())
        ymin = int(entry_ymin.get())
        xmax = int(entry_xmax.get())
        ymax = int(entry_ymax.get())
        imageC = image.copy()
        cv2.rectangle(imageC, (xmin, ymin), (xmin + xmax, ymin + ymax), (0, 255, 0), 1)
        photoimage = ImageTk.PhotoImage(Image.fromarray(imageC))
        canvas.itemconfig(image_item, image=photoimage)
        canvas.image = photoimage
    except ValueError:
        print("Error")
        # Handle the case where the entered value is not an integer
# Bind the callback to the <FocusOut> event of the Entry widget
entry_xmin.bind("<FocusOut>", on_entry_focus_out)
entry_ymin.bind("<FocusOut>", on_entry_focus_out)
entry_xmax.bind("<FocusOut>", on_entry_focus_out)
entry_ymax.bind("<FocusOut>", on_entry_focus_out)

button_frame = tk.Frame(window, height=30)  # Adjust the height as needed
button_frame.grid(row=0, column=1, padx=10, pady=10, sticky='nsew')
# Create a drop-down menu for selecting labels
selected_label = tk.StringVar()
label_names = list(label_map)
label_dropdown = ttk.Combobox(button_frame, textvariable=selected_label, values=label_names + [remove_input], height=10)
label_dropdown.set("Select a label")
label_dropdown.grid(row=0, column=0)


# Add an event handler to respond to user selections in the drop-down menu
def on_dropdown_select(event):
    user_selection_event.set()  # Signal that the user has made a selection
def continue_click():
    user_continue_event.set()
button = tk.Button(window, text="Continue", command=continue_click)
button.grid(row=1, column=1, padx=10, pady=10, sticky='se')

def add_click():
    try:
        # Get the integer value from the Entry
        xmin = int(entry_xmin.get())
        ymin = int(entry_ymin.get())
        xmax = int(entry_xmax.get())
        ymax = int(entry_ymax.get())
        tree = ET.parse(xml_path)
        root = tree.getroot()
        selected_name = selected_label.get()
        if selected_name in label_map:
            new_object = createnewObject(selected_name, xmin, ymin, xmin+ xmax, ymin + ymax )
            # Append the new object to the root
            root.append(new_object)
            ET.indent(tree, space="\t", level=0)
            tree.write(xml_path, encoding="utf-8", xml_declaration=True, method="xml")
            label_dropdown.set("Select a label")
            entry_xmin.delete(0, tk.END)
            entry_xmin.insert(0, "0")
            entry_ymin.delete(0, tk.END)
            entry_ymin.insert(0, "0")
            entry_xmax.delete(0, tk.END)
            entry_xmax.insert(0, "0")
            entry_ymax.delete(0, tk.END)
            entry_ymax.insert(0, "0")
        user_selection_event.clear()
    except ValueError:
        print("Error")

# Add the first button to the bottom right corner
button1 = tk.Button(window, text="Add", command=add_click)
button1.grid(row=1, column=0, padx=10, pady=10, sticky='se')

label_dropdown.bind("<<ComboboxSelected>>", on_dropdown_select)
def fun():
    global image, image_item, xml_path
    # Iterate over each file in the image folder
    for filename in os.listdir(image_folder):
        # Check if the file is an image (based on file extension)
        if any(filename.lower().endswith(ext) for ext in image_extensions):
            # Load and display the image
            image_path = os.path.join(image_folder, filename)
            image = cv2.imread(image_path)
            image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
            photoimage = ImageTk.PhotoImage(Image.fromarray(image))
            
            # Display the image on the canvas
            image_item = canvas.create_image(0, 0, anchor=tk.NW, image=photoimage)
            canvas.image = photoimage 
            
            annotation_filename = os.path.splitext(filename)[0] + annotation_extension
            xml_path = os.path.join(image_folder, annotation_filename)
            tree = ET.parse(xml_path)
            root = tree.getroot()

            image_filename = root.find("filename").text
            image_width = int(root.find("size/width").text)
            image_height = int(root.find("size/height").text)
            for obj_elem in root.findall("object"):
                name = obj_elem.find("name")

                xmin = int(obj_elem.find("bndbox/xmin").text)
                ymin = int(obj_elem.find("bndbox/ymin").text)
                xmax = int(obj_elem.find("bndbox/xmax").text)
                ymax = int(obj_elem.find("bndbox/ymax").text)

                rect = (xmin, ymin, xmax, ymax)
                selected_name = ""
                if name is not None and name.text == "Unknown":
                    imageC = image.copy()
                    cv2.rectangle(imageC, (xmin, ymin), (xmax, ymax), (0, 255, 0), 1)
                    photoimage = ImageTk.PhotoImage(Image.fromarray(imageC))
                    canvas.itemconfig(image_item, image=photoimage)
                    canvas.image = photoimage
                    entry_xmin.delete(0, tk.END)
                    entry_xmin.insert(0, str(xmin))
                    entry_ymin.delete(0, tk.END)
                    entry_ymin.insert(0, str(ymin))
                    entry_xmax.delete(0, tk.END)
                    entry_xmax.insert(0, str(xmax))
                    entry_ymax.delete(0, tk.END)
                    entry_ymax.insert(0, str(ymax))
                    user_selection_event.wait()
                    user_selection_event.clear() 
                    # Extract the selected label name
                    selected_name = selected_label.get()
                    if selected_name == remove_input:
                        root.remove(obj_elem)
                    # Move the image to the corresponding subfolder
                    if selected_name in label_map:
                        name.text = selected_name
                    label_dropdown.set("Select a label")
                    entry_xmin.delete(0, tk.END)
                    entry_xmin.insert(0, "0")
                    entry_ymin.delete(0, tk.END)
                    entry_ymin.insert(0, "0")
                    entry_xmax.delete(0, tk.END)
                    entry_xmax.insert(0, "0")
                    entry_ymax.delete(0, tk.END)
                    entry_ymax.insert(0, "0")
                if selected_name != remove_input:
                    cv2.rectangle(image, (xmin, ymin), (xmax, ymax), (255, 0, 0), 1)
                    photoimage = ImageTk.PhotoImage(Image.fromarray(image))
                    canvas.itemconfig(image_item, image=photoimage)
                    canvas.image = photoimage

            tree.write(xml_path, encoding="utf-8", xml_declaration=True)
            user_continue_event.wait()
            user_continue_event.clear()

            


# Create and start a thread to run the fun_in_thread function
fun_thread = threading.Thread(target=fun)
fun_thread.start()
window.mainloop()
