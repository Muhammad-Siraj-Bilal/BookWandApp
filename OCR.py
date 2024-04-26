import os
import RPi.GPIO as GPIO
import time
import signal
import sys
import firebase_admin
from picamera import PiCamera
from google.cloud import vision_v1 as vision
from firebase_admin import credentials as firebase_credentials, firestore

# Initialize Firebase Admin using Firebase credentials
firebase_cred = firebase_credentials.Certificate("/home/bluemoon/google/book-wand.json")
firebase_admin.initialize_app(firebase_cred)
db = firestore.client()

def detect_text(path):
    """
    Detects text in the image file located at 'path' and returns the detected sentences.
    """
    # Set Google Cloud credentials for Vision API
    os.environ['GOOGLE_APPLICATION_CREDENTIALS'] = os.path.abspath("/home/bluemoon/google/ocr.json")
    client = vision.ImageAnnotatorClient()
    with open(path, 'rb') as image_file:
        content = image_file.read()
    image = vision.Image(content=content)
    response = client.text_detection(image=image)
    texts = response.text_annotations
    sentences = []
    if texts:
        sentences = texts[0].description.split('\n')
    if response.error.message:
        raise Exception(response.error.message)
    return sentences

def capture_image_and_detect_text():
    """
    Captures multiple images at 10-second intervals for a maximum of 4 pictures.
    Detects text and updates output.txt only with unique sentences not previously detected.
    """
    output_file_path = 'output.txt'
    if os.path.exists(output_file_path):  # Check if the file exists
        os.remove(output_file_path)  # Delete the file if it exists
        print(f"{output_file_path} deleted successfully.")
        
    detected_sentences = set()  # Maintain a set of detected sentences
    print("Starting image capture and text detection...")
    
    # Use 'with' statement to ensure PiCamera is properly closed after use
    with PiCamera() as camera:
        camera.start_preview()
        time.sleep(5)  # Warm-up time for the camera
        try:
            for i in range(1, 5):  # Adjusted range to capture up to 4 images
                image_path = f'image_{i}.jpg'
                camera.capture(image_path)
                print(f"Captured {image_path}")
                time.sleep(7)  # Wait for 10 seconds before the next capture
                
                sentences = detect_text(image_path)
                new_sentences = [sentence for sentence in sentences if sentence not in detected_sentences]
                if new_sentences:  # If there are new unique sentences
                    with open('output.txt', 'a') as f:  # Append new sentences to output.txt
                        for sentence in new_sentences:
                            f.write(f"{sentence}\n")
                            detected_sentences.add(sentence)  # Add new sentences to the set
                    print("Updated sentences saved to output.txt")
                else:
                    print("No new unique sentences detected.")
        finally:
            camera.stop_preview()




def update_firestore():
    """
    Resets all documents' Available status to 'False' and Location to '0' as strings,
    then updates based on the sentences from 'output.txt'.
    """
    with open('output.txt', 'r') as file:
        sentences = [line.strip() for line in file.readlines()]

    doc_ids = [
        "A Good Girls Guide to Murder",
        "Alone With You In The Ether",
        "Atomic Habits",
        "Beautiful World, Where Are You",
        "Harry Potter and the Cursed Child",
        "Inferno",
        "Jane Eyre",
        "Normal People",
        "Tales From Shakespeare",
        "The Daily Laws",
        "The Midnight Gang",
        "The Night Circus",
        "The Pickwick Papers",
        "The Seven Husbands of Evelyn Hugo",
        "The Silent Patient",
        "The Sun is Also a Star"
    ]

    # Reset all documents to Available='False' and Location='0'
    for doc_id in doc_ids:
        db.collection('Books').document(doc_id).update({"Availability": 'False', "BookNumber": '0'})

	# Initialize location counter
    location_counter = 1

   # Update documents based on sentences in output.txt
    for sentence in sentences:
        if sentence in doc_ids:
            db.collection('Books').document(sentence).update({
                "Availability": 'True',
                "BookNumber": str(location_counter)  # Use the location counter for the Location
            })
            print(f"Updated {sentence}: Available='True', BookNumber='{location_counter}'")
            location_counter += 1  # Increment the counter for the next document
        else:
            print(f"No matching document for sentence: '{sentence}'")

def remove_duplicates_from_file(file_path):
    """
    Removes duplicate lines from the specified file.
    """
    seen = set()  # set for fast O(1) amortized lookup
    unique_lines = []  # List to store unique lines

   # Read from the original file and collect unique lines
    with open(file_path, 'r') as in_file:
        for line in in_file:
            # Check if the line is not in seen and has at least 7 characters (excluding newline character)
            if line not in seen and len(line.strip()) >= 7:
                seen.add(line)
                unique_lines.append(line)

    # Write the unique lines to a new file
    with open(file_path , 'w') as out_file:
        out_file.writelines(unique_lines)
    
    print("Duplicates removed from", file_path)

def signal_handler(sig, frame):
    print('Stopping...')
    GPIO.cleanup()  # Clean up GPIO
    sys.exit(0)

def check_gpio_and_execute():
    # Setup GPIO
    GPIO.setmode(GPIO.BCM)
    trigger_pin = 17  # Set your GPIO pin here
    GPIO.setup(trigger_pin, GPIO.IN)

    try:
        while True:
            if GPIO.input(trigger_pin):
                capture_image_and_detect_text()
                remove_duplicates_from_file('output.txt')
                update_firestore()
                time.sleep(10)  # Adjust delay as needed to avoid re-triggering
            else:
                time.sleep(0.1)  # Sleep to reduce CPU usage when idle
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        GPIO.cleanup()  # Ensure GPIO is cleaned up regardless of error
        
    # Register the signal handler
signal.signal(signal.SIGINT, signal_handler)


if _name_ == "_main_":
	check_gpio_and_execute()
