er := ErrorServer new.
Field errorServer: er.
Forest roadCost: 10.
River roadCost: 8.
f := Forest new.
r := River new.
f buildRoad: 8.
r buildRoad: 8.
f getDD.
f putDD: 3.
er currentMessage