from sklearn.preprocessing import MinMaxScaler
from sklearn.preprocessing import MaxAbsScaler
from sklearn.preprocessing import StandardScaler
from sklearn.preprocessing import RobustScaler
from sklearn.preprocessing import Normalizer
from sklearn.preprocessing import QuantileTransformer
from sklearn.preprocessing import PowerTransformer

def scale(scaler, data):

    if scaler == 'MinMax':
        scaling = MinMaxScaler().fit(data)
    elif scaler == 'MaxAbs':
        scaling = MaxAbsScaler().fit(data)
    elif scaler == 'Standard':
        scaling = StandardScaler().fit(data)
    elif scaler == 'Robust':
        scaling = RobustScaler(quantile_range=(25, 75)).fit(data)
    elif scaler == 'Normal':
        scaling = Normalizer().fit(data)
    elif scaler == 'QuantileGaussian':
        scaling = QuantileTransformer(output_distribution='normal').fit(data)
    elif scaler == 'QuantileUniform':
        scaling = QuantileTransformer(output_distribution='uniform').fit(data)
    elif scaler == 'Power':
        scaling = PowerTransformer(method='yeo-johnson').fit(data)

    return scaling
