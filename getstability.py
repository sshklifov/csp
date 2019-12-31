import numpy as np
import pandas as pd
import joblib

df = pd.read_csv("./input.csv")
isleaf = df.apply(axis=1, func = lambda x: x.height == 0)

X = pd.DataFrame([df.subtree_cnt/df.n, df.depth/df.avg_depth, df.height/df.avg_height, isleaf, df.max_depth, df.max_height]).transpose()
model = joblib.load('predict.model')

mids = [-0.10043076827441082, 0.10858725933695426, 0.30766390145470374, 0.47356110321949496, 0.611808771356821, 0.7270151614712594, 0.8230204865666246]

predict = pd.Series(model.predict_proba(X).dot(mids))
predict.to_csv("output.csv", index=False, header=False);
