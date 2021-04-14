import pandas as pd
from sklearn.neighbors import KNeighborsClassifier
from sklearn import preprocessing


def location_mapper(location_codes, location_names):
    '''Maps the location name with associated label encoder code'''
    mapper = {}
    for index, code in enumerate(location_codes):
        mapper[code] = location_names[index]
    return mapper


def calculate(trf_data):
    # trf_data data structure : {"AP1": value1, "AP2": value2, "AP3": value3, "AP4": value4, "eduroam": value5}

    # prepare for data_train
    df = pd.read_csv('RSSI-dataset3.csv')
    df = df[['AP1', 'AP2', 'AP3', 'AP4', 'eduroam', 'Location']]

    # preprocessing data train
    label_encoder = preprocessing.LabelEncoder()
    location = label_encoder.fit_transform(list(df['Location']))
    rssi_1_train = pd.to_numeric(df['AP1'], downcast='float')
    rssi_2_train = pd.to_numeric(df['AP2'], downcast='float')
    rssi_3_train = pd.to_numeric(df['AP3'], downcast='float')
    rssi_4_train = pd.to_numeric(df['AP4'], downcast='float')
    rssi_5_train = pd.to_numeric(df['eduroam'], downcast='float')

    dict_loc_names = location_mapper(location, list(df['Location']))

    x_train = list(zip(rssi_1_train, rssi_2_train, rssi_3_train, rssi_4_train, rssi_5_train))
    y_train = list(location)
    # print(x_train)

    # prepare for data test
    x_test = [(float(trf_data["AP1"]), float(trf_data["AP2"]), float(trf_data["AP3"]), float(trf_data["AP4"]),
               float(trf_data["eduroam"]))]
    # print(x_test)

    # modelling
    model = KNeighborsClassifier(n_neighbors=3)
    model.fit(x_train, y_train)

    # predict
    predict = model.predict(x_test)

    # print(dict_loc_names[predict[0]])

    return dict_loc_names[predict[0]]





