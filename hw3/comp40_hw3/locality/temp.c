void rotate_270(int i, int j, A2Methods_UArray2 array, A2Methods_Object *elem, 
        void *cl)
{
        struct Closure *mcl = cl;
        int newI = j;
        int newJ = mcl->methods->height(array) - i - 1;
        void *pixelptr = mcl->methods->at(mcl->array, newI, newJ);
        Pnm_rgb pixelNew = pixelptr;
        Pnm_rgb pixelOld = elem;
        *pixelNew = *pixelOld;
}
