#include "stdio.h"
#include <memory>
#include "arboretum_wrapper.h"
#include "../src/io/io.h"
#include "../src/core/param.h"
#include "../src/core/garden.h"

using namespace std;
using namespace arboretum;
using namespace arboretum::io;
using namespace arboretum::core;

namespace arboretum {
  namespace wrapper {
    extern "C" const char* ACreateFromDanseMatrix(const float *data,
                                          int nrow,
                                          int ncol,
                                          float  missing,
                                          VoidPointer *out){
      try{
        DataMatrix* mat = new DataMatrix(nrow, ncol);
        #pragma omp parallel for
        for(int i = 0; i<ncol*nrow; ++i){
            mat->data[i % ncol][i / ncol] = data[i];
          }
        *out = static_cast<VoidPointer>(mat);
        return NULL;
      } catch(const char* error){
        return error;
      }
    }

    extern "C" const char* ASetY(VoidPointer data,
                                 const float *y){
   try{
        DataMatrix *data_ptr = static_cast<DataMatrix*>(data);
        data_ptr->y_hat.reserve(data_ptr->rows);
        #pragma omp parallel for
        for(size_t i = 0; i < data_ptr->rows; ++i){
          data_ptr->y_hat[i] = y[i];
        }
        return NULL;
      } catch(const char* error){
        return error;
      }
    }

    extern "C" const char* AInitGarden(int obj,
                                       int depth,
                                       int min_child_weight,
                                       float colsample_bytree,
                                       float eta,
                                       VoidPointer *out){

      try{
        TreeParam *param = new TreeParam((Objective)obj, depth, min_child_weight, colsample_bytree, eta);
        Garden* source = new Garden(*param);
        *out = static_cast<VoidPointer>(source);
        return NULL;
      } catch(const char* error){
        return error;
      }
    }

    extern "C" const char* AGrowTree(VoidPointer garden,
                                     VoidPointer data,
                                     const float *grad){
      try{
      io::DataMatrix *data_ptr = static_cast<DataMatrix*>(data);
      Garden *g = static_cast<Garden*>(garden);
      g->GrowTree(data_ptr, (float*)grad);
      return NULL;
      } catch(const char* error){
        return error;
      }
    }

    extern "C" const char* APredict(VoidPointer garden,
                                            VoidPointer data,
                                            const float **out){
      try{
        io::DataMatrix *data_p = static_cast<DataMatrix*>(data);
        Garden *garden_p = static_cast<Garden*>(garden);

        std::vector<float> result;
        garden_p->Predict(data_p, result);

        float* p = new float[result.size()];
        #pragma omp parallel for
        for(size_t i = 0; i < result.size(); ++i){
            p[i] = result[i];
          }
        *out = p;
        return NULL;
      } catch(const char* error){
        return error;
      }
    }

    extern "C" const char* AFreeDMatrix(VoidPointer ptr){
      delete static_cast<DataMatrix*>(ptr);
      return NULL;
    }
    extern "C" const char* AFreeGarden(VoidPointer ptr){
      delete static_cast<Garden*>(ptr);
      return NULL;
    }
    }
}
