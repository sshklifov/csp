import numpy as np
import pandas as pd
from sklearn.ensemble import RandomForestClassifier

import joblib
import warnings
warnings.simplefilter(action='ignore', category=FutureWarning)

df = pd.read_csv("./input.csv")
df['isleaf'] = df.apply(axis=1, func = lambda x: x.height == 0)

X = pd.DataFrame([df.subtree_cnt, df.depth, df.height, df.isleaf, df.n]).transpose()
model = joblib.load('predict.model')

mids = [-0.10332790069792004, 0.10858725933695426, 0.30766390145470374, 0.47356110321949496, 0.611808771356821, 0.7270151614712594, 0.8230204865666246]

predict = pd.Series(model.predict_proba(X).dot(mids))
predict.to_csv("output.csv", index=False);
