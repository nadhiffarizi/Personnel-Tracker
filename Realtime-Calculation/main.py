from kafka import KafkaConsumer
from Predictor import calculate
import requests
import json


def stream():
    consumer = KafkaConsumer('topic1', group_id='my_group', bootstrap_servers=['localhost:9092'], api_version=(0, 10),
                             auto_offset_reset='earliest')
    for message in consumer:
        # message value and key are raw bytes -- decode if necessary!
        # e.g., for unicode: `message.value.decode('utf-8')`
        transformed_data = parse_data(message.value.decode('utf-8'))





def parse_data(msg):
    try:
        raw_data = json.loads(msg)
        _dict = {"AP1": raw_data["AP1"], "AP2": raw_data["AP2"], "AP3": raw_data["AP3"], "AP4": raw_data["AP4"],
                 "eduroam": raw_data["eduroam"]}
        calculated_location = calculate(_dict)
        transformed_data = {"time": raw_data["time"], "ID": raw_data["ID"], "Location": calculated_location}
        print(transformed_data)

        send_data(transformed_data)
        # return type: dictionary
        return transformed_data
    except Exception:
        raise Exception("Error parsing to Json")


def send_data(data_to_send):
    '''Send data using python requests to nifi aghnia'''
    # print("Data to send: ", data_to_send)
    url = 'http://192.168.42.10:7001/contentListener'
    response = requests.post(url, json=data_to_send)
    print(response.status_code)


if __name__ == "__main__":
    stream()
