Questions to Answer
---

#### 1) Does our understanding of the search window concept correspond with the implementation?

~~Our understanding is the windows' centers are specified as points on concentric circles around the
current target location. The windows are 16 x 16 pixels. There should be 81 search locations.~~

There are actually two search algorithms implemented: one for tracking, and one for updating the
learner.

##### Sampling for tracking

This creates a sample window centered at every pixel within the search radius. The search radius is
specified by the user in a configuration file. The windows are set to 80 x 80 pixels at compile
time. Any windows which have any part outside the image are discarded.

##### Sampling for udpating

This performs the polar grid sampling similar to what is described in the paper. The radius searched
is actually twice the search radius used for tracking sampling. Sampling at five radii, and 16
angles is set at compile time. Again, the sample windows are 80 x 80 pixels.

#### 2) Verify the implementation's feature extraction.

~~Our understanding from the paper is 192 Haar and histogram features.~~

The implementation allows selecting a combination of Haar, histogram, and raw features. The code
appears to extract the features as described in the paper.

192 Haar features are generated from each sample window.

480 histogram features are generated from each sample window.

256 raw features are generated from each sample window.

#### 3) Verify the SVM training input.

~~Our understanding from the paper is features extracted from 81 search windows.~~

Updating the learner starts by extracting 81 samples from a polar grid as described in the paper in
Section 3.1 (page 268). The grid consists of five radii, and 16 angles, resulting in 80 locations.
Sampling the current bounding box location makes 81. Any samples which overlap the frame edge are
discarded. The input consists of features extracted from each sample (x), and the sample's
translation from the bounding box (y).

##### a) What is the label information?

The labels are the translations from the bounding box just detected, to the sample windows.

#### 4) How is the best window selected?

After the tracker has determined the sample windows, they are fed to the SVM. The SVM extracts
features from the windows. Each feature vector, with the window's transformation label, are then
evaluated against the existing support vectors. For each support vector, the kernel function is
evaluated with the sample feature vector, which is then scaled by the support vector's β value. The
sum of these operations is accumulated as the result for that feature vector:

```
for each window
    extract feature vector, x
    calculate transformation, y
    initialize score f = 0
    for each support vector, s
        evaluate k(x, s.x)
        f += s.β * k(x, s.x)
    end for
end for
```

At this point, the implementation has a list of scores for each candidate window. The window with
the maximum score is the new target bounding box.

---

#### Where in the code are the following points from the paper?

##### Equation 4

**y**<sub>t</sub> = argmax<sub>*Y*</sub> F(**x**<sub>t</sub><sup>**p**</sup>, **y**)

The implementation is spread over a few places. F is implemented in `LaRank::Evaluate()`,
[LaRank.cpp, lines 62-71](https://github.com/brobeson/struck/blob/cs_7680/src/LaRank.cpp#L62-L71)

Locating the maximum F, and its associated transformation **y**, is in `Tracker::Track()`,
[Tracker.cpp, lines 174-183](https://github.com/brobeson/struck/blob/cs_7680/src/Tracker.cpp#L174-L183).

##### Equation 5

min<sub>**w**</sub> 0.5 ||**w**||<sup>2</sup> + C sum<sub>i=1</sub><sup>n</sup>ξ<sub>i</sub>

This equation is not in the implementation. Equations derived from it are.

##### Algorithm 1 - SMOStep

This algorithm is implemented in `LaRank::SMOStep()`,
[LaRank.cpp, lines 162 - 217](https://github.com/brobeson/struck/blob/cs_7680/src/Tracker.cpp#L174-L183)

##### Algorithm 2 - Struck

Algorithm 2 is an abstract representation of the process as a whole. Most of these algorithm steps
are implemented in the LaRank class, in
[LaRank.cpp](https://github.com/brobeson/struck/blob/cs_7680/src/LaRank.cpp)

---

#### Potential areas for improvement:

##### Could the SVM be converted to fuzzy?

Probably. I suspect the primary gain from this would be to more quickly remove support vectors which
exceed the budget.

##### Does the SVM maintain history? If not, could that be added?

It does. It keeps support vectors from previous frames, so long as the budget is not exceeded. When
the budget is exceeded, the support vector with least influence on the weights is removed.

##### Can bad support vectors be eliminated in another way?

Perhaps by using a fuzzy SVM.

##### Why throw out windows on the edge of the image?

I plan to run some tests and see if the tracker loses the object near the edge.

##### Complete occlusion confuses the tracker.

I was showing the application to my daughters, tracking their faces. I noticed that if I passed my
hand in front of the tracked object, the application switched to tracking my hand. When I moved my
hand back over the tracked object, the application returned to tracking the original object.

##### Is there a better way to select which support vectors to update?

The algorithm as described, and implemented, randomly selects a support vector to update during a
few steps. Typically, randomness inhibits consistent runs of the algorithm. This is mitigated
somewhat here, by forcing the user to supply a random seed at run time.

##### Use SIFT features, or something similar, to account for scale.

This was Amir's idea after my presentation. As I understood it, the SIFT features would replace
Haar, histogram, and raw features.
