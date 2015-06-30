smartDM: C++ Stream Data Mining 
=================

smartDM implements extremely fast streaming decision trees in C++ for big data streams.

The main advantages of smartDM over other C/C++ data stream libraries are the following:

- Evaluation and learners are separated, not linked together.
- It contains several methods for learning numeric attributes.
- It is easy to extend and add new methods.
- The adaptive decision tree is more accurate and does not need an expert user to choose optimal parameters to use.  
- It contains powerful ensemble methods. 
- It is much faster and uses less memory.


## Getting Started


Getting Started

First download and build smartDM:

```
git clone https://github.com/huawei-noah/smartDM.git
cd smartDM
make
```

Download a dataset:

```
wget "http://downloads.sourceforge.net/project/moa-datastream/Datasets/Classification/covtypeNorm.arff.zip"
unzip covtypeNorm.arff.zip
```

Evaluate the dataset:

```
./SmartDM "EvaluatePrequential -l (HoeffdingTree -l NBAdaptive) -r ArffReader -ds covtypeNorm.arff -e (BasicClassificationEvaluator -f 100000)"
```


## Methods

smartDM for C++ executes tasks. Tasks can be evaluation tasks as "EvaluatePrequential" or "EvaluateHoldOut" and the parameters needed are a learner, a stream reader, and an evaluator.

The methods currently implemented are: Naive Bayes, Perceptron, Logistic Regression, Perceptron, Majority Class, Hoeffding Tree, Hoeffding Adaptive Tree, and Bagging.

The stream readers currently implemented support Arff, C45, and LibSVM formats.



