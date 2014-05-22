#!/usr/bin/python2.7
import pykst as kst

client=kst.Client("TestPlugins")

v1 = client.new_generated_vector(0, 10, 10)
v2 = client.new_generated_vector(0, 20, 10)

c1 = client.new_curve(v1, v2)

v4 = c1.x_vector()
print v4.handle
print v4.name(), v4.value(0), v4.value(1)

lf1 = client.new_linear_fit(v1, v2)

v3 = lf1.parameters()
print v3.handle
print v3.name(), v3.value(0), v3.value(1)

v4 = lf1.fit()
print v4.name(), v4.value(0), v4.value(1)

v5 = lf1.residuals()
print v5.name(), v5.value(0), v5.value(1)

v6 = lf1.covariance()
print v6.name(), v6.value(0), v6.value(1)

x1 = lf1.reduced_chi2()
print x1.name(), x1.value()

print lf1.slope(), lf1.intercept()

x2 = client.new_generated_scalar(3)
pf1 = client.new_polynomial_fit(x2, v1, v2)
