import numpy as np
import pandas as pd
import joblib

df = pd.read_csv("./input.csv")

isleaf = df.apply(axis=1, func = lambda x: x.height == 0)

bdepth = df.apply(axis=1, func = lambda x: x.depth > x.avg_depth).rename("bdepth")
bheight = df.apply(axis=1, func = lambda x: x.height > x.avg_height).rename("bheight")

sdepth = df.apply(axis=1, func = lambda x: x.depth / x.max_depth).rename("sdepth")
sheight = df.apply(axis=1, func = lambda x: x.height / x.max_height).rename("sheight")

X = pd.DataFrame([df.subtree_cnt/df.n, sdepth, bdepth, sheight, bheight, isleaf]).transpose()
model = joblib.load('predict.model')
predict = model.predict(X)

pd.Series(predict).to_csv("output.csv", index=False, header=False);
