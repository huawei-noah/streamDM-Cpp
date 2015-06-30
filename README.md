streamDM-C++: C++ Stream Data Mining 
=================

streamDM for C++ implements extremely fast streaming decision trees in C++ for big data streams. It is a project developed at Huawei Noah's Ark Lab. streamDM-C++ is licensed under Apache Software License v2.0.

The main advantages of streamDM for C++ over other C/C++ data stream libraries are the following:

- Evaluation and learners are separated, not linked together.
- It contains several methods for learning numeric attributes.
- It is easy to extend and add new methods.
- The adaptive decision tree is more accurate and does not need an expert user to choose optimal parameters to use.  
- It contains powerful ensemble methods. 
- It is much faster and uses less memory.


## Getting Started


Getting Started

First download and build streamDM for C++:

```
git clone https://github.com/huawei-noah/streamDM-cpp.git
cd streamDM-cpp
make
```

Download a dataset:

```
wget "http://downloads.sourceforge.net/project/moa-datastream/Datasets/Classification/covtypeNorm.arff.zip"
unzip covtypeNorm.arff.zip
```

Evaluate the dataset:

```
./streamdm-cpp "EvaluatePrequential -l (HoeffdingTree -l NBAdaptive) -r ArffReader -ds covtypeNorm.arff -e (BasicClassificationEvaluator -f 100000)"
```


## Methods

streamDM for C++ executes tasks. Tasks can be evaluation tasks as "EvaluatePrequential" or "EvaluateHoldOut" and the parameters needed are a learner, a stream reader, and an evaluator.

The methods currently implemented are: Naive Bayes, Perceptron, Logistic Regression, Perceptron, Majority Class, Hoeffding Tree, Hoeffding Adaptive Tree, and Bagging.

The stream readers currently implemented support Arff, C45, and LibSVM formats.



