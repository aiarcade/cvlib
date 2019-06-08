#include "detector.h"


int Detector::
max(int a, int b)
{
    return (a >= b) ? a : b;
}

int Detector::
min(int a, int b)
{
    return (a <= b) ? a : b;
}

int Detector::
round(float value)
{
    return (int)(value + (value >= 0 ? 0.5 : -0.5));
}

int Detector::
abs(int n)
{
    return (n >= 0) ? n : -n;
}

int
Detector::detect( unsigned char *y,Object *fa, int *maxfaces)
{
    detectorGenIntegralImage(this->integral_img, y, this->width, this->height);
    objectDetectorLbpDetect(this->l, this->integral_img, fa, maxfaces);

    return 0;
}

void
Detector::detectorGenIntegralImage(unsigned int *i_data, unsigned char *data, int width, int height)
{
    // first row only
    int i, j;
    unsigned int rs = 0;
    for (j = 0; j < width; j++) {
        rs += data[j];
        i_data[j] = rs;
    }
    // remaining cells are sum above and to the left
    for (i = 1; i < height; ++i) {
        rs = 0;
        for (j = 0; j < width; ++j) {
            rs += data[i * width + j];
            i_data[i * width + j] = rs + i_data[(i - 1) * width + j];
        }
    }
}
int Detector::lbpDetect(Lbp *l, unsigned int *img, int x, int y, int width, int height, float scale)
{
    /* loop for all stages */
    float threshold;
    int i, j;
    for (i = 0; i < l->data.num_stages; i++) {
        /* loop all weak classifiers */
        threshold = 0;
        for (j = 0; j < l->data.s[i].num_weak_classifiers; j++) {
            threshold += l->lbpClassify(l->data.r, &l->data.s[i].classifiers[j], img, x, y, width, height, scale);
        }
        if (threshold < l->data.s[i].stage_threshold) {
            /* not matched */
            return 0;
        }
    }
    /* here we pass all the stages and found a match */
    return 1;
}

int
Detector::objectDetectorLbpDetect(Lbp *l, unsigned int *img, Object *fa, int *maxfaces)
{
    int i;
    int found;
    int width, height;
    width = l->width;
    height = l->height;


    for (i = 0; i < l->tasks.size(); i++) {
        found = lbpDetect(l, img, l->tasks[i].x, l->tasks[i].y, width, height, l->tasks[i].scale);
        if (found) {

            addLbpObject(l, l->tasks[i].x, l->tasks[i].y, l->tasks[i].scale);
        }
    }
    /* merge overlapped rectangles */
    ObjectDetectorGroupRectangle(l->detected_r, l->para.group_threshold, l->para.eps);

    /* return faces detected after merging */
    if (l->detected_r.size() > (unsigned int)*maxfaces) {
        //ALOGW("User provided maxface size not large enough");
    } else {
        *maxfaces = l->detected_r.size();
    }

    //ALOGD("LBP tested: %ld", l->tasks.size());

    for (i = 0; i < *maxfaces; i++) {
        fa[i].x = l->detected_r[i].x;
        fa[i].y = l->detected_r[i].y;
        fa[i].width = l->detected_r[i].w;
        fa[i].height = l->detected_r[i].h;
        fa[i].confidence_level = 50; /* TODO */
    }
    l->detected_r.clear();

    return 0;
}

void
Detector::addLbpObject(Lbp *l, int x, int y, float scale)
{
    LbpRect r;
    r.x = x;
    r.y = y;
    r.w = l->data.feature_width * scale;
    r.h = l->data.feature_height * scale;
    l->detected_r.push_back(r);
}

Lbp *
Detector::ObjectDetectorLbpCreate(int width, int height, int minimum_face_width)
{
    Lbp *l = new Lbp();
    int ret;
    l->para.scaling_factor = 1.125;
    l->para.step_scale_x = 8;
    l->para.step_scale_y = 8;
    l->para.tracking_scale_down = 0.5;
    l->para.tracking_scale_up = 1.5;
    l->para.tracking_offset = 0.5;
    l->para.group_threshold = 2;
    l->para.eps = 0.2;
    l->para.min_face_width = minimum_face_width;
    l->width = width;
    l->height = height;

    ret=loadLbpDataFromXML(l);

    if (ret) {

        return NULL;
    }
    initTask(l);


    return l;
}
tinyxml2::XMLElement* Detector::findElement(tinyxml2::XMLElement* row,char * name)
{
  row=row->FirstChildElement();
  while (row != NULL)
  {
    int cmp=strcmp(row->Value(),name);

    if (cmp==0)
    {

      return row;
    }
    else
    {
      row=row->NextSiblingElement();
    }

  }
  return NULL;

}
size_t Detector::splitDouble(const std::string &txt, std::vector<double> &numbers, char ch)
{


  std::stringstream iss( txt );
  double number;
  while ( iss >> number )
    numbers.push_back( number );
  return numbers.size();
}
size_t Detector::splitInt(const std::string &txt, std::vector<int> &numbers, char ch)
{

  std::stringstream iss( txt );
  int number;
  while ( iss >> number )
    numbers.push_back( number );
  
  return numbers.size();
}


int Detector::loadLbpDataFromXML(Lbp *l)
{

    int i, j;
    tinyxml2::XMLDocument doc;
    doc.LoadFile(this->cascade_file);
    tinyxml2::XMLElement* parent = doc.FirstChildElement("opencv_storage");

    tinyxml2::XMLElement *cas_ele = parent->FirstChildElement("cascade");
    tinyxml2::XMLElement * h_ele =findElement(cas_ele,"height");
    tinyxml2::XMLElement * w_ele =findElement(cas_ele,"width");
    tinyxml2::XMLElement * sn_ele =findElement(cas_ele,"stageNum");

    l->data.feature_height=atoi(h_ele->GetText());
    l->data.feature_width = atoi(w_ele->GetText());
    l->data.num_stages = atoi(sn_ele->GetText());

    if ((l->data.num_stages <= 0)) {
      l->data.num_stages = 0;
      return -EIO;
    }
    Stage * s = new Stage[l->data.num_stages];

    l->data.s = s;
    tinyxml2::XMLElement * stage_ele  =findElement(cas_ele,"stages");
    tinyxml2::XMLElement *  sym_ele=findElement(stage_ele,"_");
    while(sym_ele!=NULL){
      tinyxml2::XMLElement *  wcn_ele=findElement(sym_ele,"maxWeakCount");
      tinyxml2::XMLElement *  stgthr_ele=findElement(sym_ele,"stageThreshold");
      tinyxml2::XMLElement *  wc_ele=findElement(sym_ele,"weakClassifiers");
      int wc_count=atoi(wcn_ele->GetText());
      double stage_thr=atof(stgthr_ele->GetText());
      l->data.s[i].num_weak_classifiers=wc_count;
      l->data.s[i].stage_threshold=stage_thr;
      if ((l->data.s[i].num_weak_classifiers <= 0)) {

            l->data.s[i].num_weak_classifiers = 0;
            return -EIO;
      }
      WeakClassifier * wc = new WeakClassifier [l->data.s[i].num_weak_classifiers];
      l->data.s[i].classifiers =  wc;
      tinyxml2::XMLElement *  symc_ele=findElement(wc_ele,"_");

      j=0;
      while(symc_ele!=NULL)
      {
        tinyxml2::XMLElement *  in_ele=symc_ele->FirstChildElement("internalNodes");
        tinyxml2::XMLElement *  lf_ele=symc_ele->LastChildElement("leafValues");
        std::vector<double> leafValues;
        std::vector<int> internalNodes;
        splitInt(in_ele->GetText(),internalNodes,' ');
        splitDouble(lf_ele->GetText(),leafValues,' ');

        l->data.s[i].classifiers[j].rect_idx=internalNodes[2]; /* REVISIT sanity check for out of bound ? */
        l->data.s[i].classifiers[j].lbpmap[0]=internalNodes[3];
        l->data.s[i].classifiers[j].lbpmap[1]=internalNodes[4];
        l->data.s[i].classifiers[j].lbpmap[2]=internalNodes[5];
        l->data.s[i].classifiers[j].lbpmap[3]=internalNodes[6];
        l->data.s[i].classifiers[j].lbpmap[4]=internalNodes[7];
        l->data.s[i].classifiers[j].lbpmap[5]=internalNodes[8];
        l->data.s[i].classifiers[j].lbpmap[6]=internalNodes[9];
        l->data.s[i].classifiers[j].lbpmap[7]=internalNodes[10];
        l->data.s[i].classifiers[j].neg=leafValues[0];
        l->data.s[i].classifiers[j].pos=leafValues[1];
        symc_ele=symc_ele->NextSiblingElement();
        j++;
      }
      sym_ele=sym_ele->NextSiblingElement();
      i++;
    }

    tinyxml2::XMLElement * feature_ele  =findElement(cas_ele,"features");
    sym_ele=findElement(feature_ele,"_");
    i=0;
    while(sym_ele!=NULL){
        sym_ele=sym_ele->NextSiblingElement();
        i++;

    }

    l->data.num_rects=i;
    if (l->data.num_rects <= 0) {
        l->data.num_rects = 0;
        return -EIO;
    }



    LbpRect * r= new LbpRect[l->data.num_rects];
    l->data.r = r;
    feature_ele  =findElement(cas_ele,"features");
    sym_ele=findElement(feature_ele,"_");
    i=0;
    while(sym_ele!=NULL) {
        tinyxml2::XMLElement *  rct_ele=findElement(sym_ele,"rect");
        std::vector<int> rects;
        splitInt(rct_ele->GetText(),rects,' ');
        l->data.r[i].x = rects[0];
        l->data.r[i].y = rects[1];
        l->data.r[i].w = rects[2];
        l->data.r[i].h = rects[3];
        sym_ele=sym_ele->NextSiblingElement();
        i++;
    }


    return 0;
}
int Detector::loadLbpData(Lbp *l)
{

    int i, j;
    std::ifstream file( this->cascade_file );
    std::stringstream in;
    if ( file )
    {


        in << file.rdbuf();

        file.close();


    }

    if (in.fail()) {


            return -EIO;

    }

    in >> l->data.feature_height >> l->data.feature_width >> l->data.num_stages;

    if (in.fail() || in.eof() || (l->data.num_stages <= 0)) {
        l->data.num_stages = 0;
        //ALOGE("Unexpected end of file %s", DATA_FILE_PATH);
        return -EIO;
    }
    Stage * s = new Stage[l->data.num_stages];

    l->data.s = s;

    for (i = 0; i < l->data.num_stages; i++) {
        in >> l->data.s[i].num_weak_classifiers >> l->data.s[i].stage_threshold;
        if (in.fail() || in.eof() || (l->data.s[i].num_weak_classifiers <= 0)) {
            //ALOGE("Unexpected end of file %s, stage: %d, weak_classifiers: %d", DATA_FILE_PATH, i + 1, l->data.s[i].num_weak_classifiers);
            l->data.s[i].num_weak_classifiers = 0;
            return -EIO;
        }
        WeakClassifier * wc = new WeakClassifier [l->data.s[i].num_weak_classifiers];
        l->data.s[i].classifiers =  wc;
        for (j = 0; j < l->data.s[i].num_weak_classifiers; j++) {
            in >> l->data.s[i].classifiers[j].rect_idx /* REVISIT sanity check for out of bound ? */
               >> l->data.s[i].classifiers[j].lbpmap[0]
               >> l->data.s[i].classifiers[j].lbpmap[1]
               >> l->data.s[i].classifiers[j].lbpmap[2]
               >> l->data.s[i].classifiers[j].lbpmap[3]
               >> l->data.s[i].classifiers[j].lbpmap[4]
               >> l->data.s[i].classifiers[j].lbpmap[5]
               >> l->data.s[i].classifiers[j].lbpmap[6]
               >> l->data.s[i].classifiers[j].lbpmap[7]
               >> l->data.s[i].classifiers[j].neg
               >> l->data.s[i].classifiers[j].pos;
        }
    }
    in >> l->data.num_rects;
    if (in.fail() || in.eof() || (l->data.num_rects <= 0)) {
        l->data.num_rects = 0;
        //ALOGE("Unexpected end of file %s", DATA_FILE_PATH);
        return -EIO;
    }

    LbpRect * r= new LbpRect[l->data.num_rects];
    l->data.r = r;
    for (i = 0; i < l->data.num_rects; i++) {
        in >> l->data.r[i].x >> l->data.r[i].y >> l->data.r[i].w >> l->data.r[i].h;
    }
    if (in.fail()) {
        //ALOGE("Unexpected end of file %s", DATA_FILE_PATH);
        return -EIO;
    }

    //dump_stages_info(l);

    return 0;
}
void Detector::initTask(Lbp *l)
{
    float scale;
    float scale_max = fminf((float)l->width / l->data.feature_width, (float)l->height / l->data.feature_height);
    float scale_min = (float)l->para.min_face_width / l->data.feature_width;

    for (scale = scale_min; scale < scale_max; scale *= l->para.scaling_factor) {
        int x, y;
        float scaled_width = l->data.feature_width * scale;
        float scaled_height = l->data.feature_height * scale;
        int step_x = scaled_width / l->para.step_scale_x;
        int step_y = scaled_height / l->para.step_scale_y;
        for (x = 0; (x + scaled_width) < (l->width - 1); x += step_x) {
            for (y = 0; (y + scaled_height) < (l->height - 1); y += step_y) {
                LbpTask t;
                t.x = x;
                t.y = y;
                t.scale = scale;
                l->tasks.push_back(t);
            }
        }
    }
}

void
Detector::ObjectDetectorGroupRectangle(std::vector<LbpRect>& rect_list, int group_threshold, float eps)
{
    if (group_threshold <= 0 || rect_list.empty())
        return;

    std::vector<int> labels;

    int nclasses = partition(rect_list, labels, eps);

    std::vector<LbpRect> rrects(nclasses);
    std::vector<int> rweights(nclasses);

    int i, j, nlabels = (int)labels.size();


    for (i = 0; i < nlabels; i++) {
        int cls = labels[i];
        rrects[cls].x += rect_list[i].x;
        rrects[cls].y += rect_list[i].y;
        rrects[cls].w += rect_list[i].w;
        rrects[cls].h += rect_list[i].h;
        rweights[cls]++;
    }
    for (i = 0; i < nclasses; i++) {
        LbpRect r = rrects[i];
        float s = 1.f/rweights[i];
        rrects[i].x = round(r.x*s);
        rrects[i].y = round(r.y*s);
        rrects[i].w = round(r.w*s);
        rrects[i].h = round(r.h*s);

    }

    rect_list.clear();

    for (i = 0; i < nclasses; i++) {
        LbpRect r1 = rrects[i];
        int n1 = rweights[i];
        if( n1 <= group_threshold )
            continue;
        /* filter out small face rectangles inside large rectangles */
        for (j = 0; j < nclasses; j++) {
            int n2 = rweights[j];
            /*********************************
             * if it is the same rectangle,
             * or the number of rectangles in class j is < group threshold,
             * do nothing
             ********************************/
            if (j == i || n2 <= group_threshold)
                continue;
            LbpRect r2 = rrects[j];

            int dx = round( r2.w * eps );
            int dy = round( r2.h * eps );

            if (i != j &&
                    r1.x >= r2.x - dx &&
                    r1.y >= r2.y - dy &&
                    r1.x + r1.w <= r2.x + r2.w + dx &&
                    r1.y + r1.h <= r2.y + r2.h + dy &&
                    (n2 > max(3, n1) || n1 < 3))
                break;
        }

        if (j == nclasses) {
            rect_list.push_back(r1); // insert back r1
        }
    }
}
int Detector::partition(std::vector<LbpRect>& _vec, std::vector<int>& labels, float eps)
{
    int i, j, N = (int)_vec.size();

    LbpRect* vec = &_vec[0];

    const int PARENT=0;
    const int RANK=1;

    std::vector<int> _nodes(N*2);

    int (*nodes)[2] = (int(*)[2])&_nodes[0];

    /* The first O(N) pass: create N single-vertex trees */
    for (i = 0; i < N; i++) {
        nodes[i][PARENT]=-1;
        nodes[i][RANK] = 0;
    }

    /* The main O(N^2) pass: merge connected components */
    for (i = 0; i < N; i++) {
        int root = i;

        /* find root */
        while (nodes[root][PARENT] >= 0)
            root = nodes[root][PARENT];

        for (j = 0; j < N; j++ ) {
            if( i == j || !predicate(eps, vec[i], vec[j]))
                continue;
            int root2 = j;

            while (nodes[root2][PARENT] >= 0)
                root2 = nodes[root2][PARENT];

            if (root2 != root) {
                /* unite both trees */
                int rank = nodes[root][RANK], rank2 = nodes[root2][RANK];
                if (rank > rank2)
                    nodes[root2][PARENT] = root;
                else {
                    nodes[root][PARENT] = root2;
                    nodes[root2][RANK] += rank == rank2;
                    root = root2;
                }

                int k = j, parent;

                /* compress the path from node2 to root */
                while ((parent = nodes[k][PARENT]) >= 0) {
                    nodes[k][PARENT] = root;
                    k = parent;
                }

                /* compress the path from node to root */
                k = i;
                while ((parent = nodes[k][PARENT]) >= 0) {
                    nodes[k][PARENT] = root;
                    k = parent;
                }
            }
        }
    }

    /* Final O(N) pass: enumerate classes */
    labels.resize(N);
    int nclasses = 0;

    for (i = 0; i < N; i++) {
        int root = i;
        while (nodes[root][PARENT] >= 0)
            root = nodes[root][PARENT];
        /* re-use the rank as the class label */
        if (nodes[root][RANK] >= 0)
            nodes[root][RANK] = ~nclasses++;
        labels[i] = ~nodes[root][RANK];
    }

    return nclasses;
}
int Detector::predicate(float eps, LbpRect& r1, LbpRect& r2)
{
    float delta = eps*(min(r1.w, r2.w) + min(r1.h, r2.h))*0.5;
    return abs(r1.x - r2.x) <= delta &&
        abs(r1.y - r2.y) <= delta &&
        abs(r1.x + r1.w - r2.x - r2.w) <= delta &&
        abs(r1.y + r1.h - r2.y - r2.h) <= delta;
}

void Detector::printClassifierInfo(Lbp *l)
{
  int j=0;
  int i=0;
  for(i=0;i<1;i++)
  {
    std::cerr<<"Stage "<<i<<"\n";
    std::cerr<<"NC"<<l->data.s[i].num_weak_classifiers<<"\n";
    std::cerr<<"THR"<<l->data.s[i].stage_threshold<<"\n";
    for (j = 0; j < l->data.s[i].num_weak_classifiers; j++) {
           std::cerr<<l->data.s[i].classifiers[j].rect_idx << " " /* REVISIT sanity check for out of bound ? */
           <<l->data.s[i].classifiers[j].lbpmap[0] << " "
           <<l->data.s[i].classifiers[j].lbpmap[1] << " "
           <<l->data.s[i].classifiers[j].lbpmap[2] << " "
           <<l->data.s[i].classifiers[j].lbpmap[3] << " "
           <<l->data.s[i].classifiers[j].lbpmap[4] << " "
           <<l->data.s[i].classifiers[j].lbpmap[5] << " "
           <<l->data.s[i].classifiers[j].lbpmap[6] << " "
           <<l->data.s[i].classifiers[j].lbpmap[7] << " "
           <<l->data.s[i].classifiers[j].neg <<" "
           <<l->data.s[i].classifiers[j].pos<<"\n";
    }
  }
  for (i = 0; i < l->data.num_rects; i++) {
      std::cerr << l->data.r[i].x << l->data.r[i].y << l->data.r[i].w << l->data.r[i].h<<"-";

    }

}
