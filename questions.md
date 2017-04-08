Questions to Answer
---

#### 1) Does our understanding of the search window concept correspond with the implementation?

Our understanding is the windows' centers are specified as points on concentric circles around the
current target location. The windows are 16 x 16 pixels. There should be 81 search locations.

#### 2) Verify the implementation's feature extraction.

Our understanding from the paper is 192 Haar and histogram features.

#### 3) Verify the SVM training input.

Our understanding from the paper is features extracted from 81 search windows.

##### a) What is the label information?

We currently think it's the transformation from the object location at `t-1` to the search window.

#### 4) How is the best window selected?

---

#### Where in the code are the following points from the paper?

##### Equation 4

##### Equation 5

##### Algorithm 1 - SMOStep

##### Algorithm 2 - Struck

---

#### Potential areas for improvement:

##### Could the SVM be converted to fuzzy?

##### Does the SVM maintain history? If not, could that be added?

##### Can bad support vectors be eliminated in another way?
