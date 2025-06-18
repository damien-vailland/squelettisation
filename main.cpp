/****************************************************************************
 * Copyright (C) 2016 Universite de Rennes. All rights reserved.
 *
 * This software was developed at:
 * Universite de Rennes
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 *
 * This file uses the ViSP library.
*****************************************************************************/

/****************************************************************************
 * NOMS - PRENOMS:
 *  - PLESSIS Axel
 *	- VAILLAND Damien
 * 
 * Date : 08/12/24
 *****************************************************************************/


#include <iostream>

#include <visp/vpConfig.h>
#include <visp/vpDebug.h>

#include <visp/vpImage.h>
#include <visp/vpImageIo.h>
#include <visp/vpDisplayX.h>


using namespace std;



/**
 * @brief affiche une image à l'écran à la position (posX,posY) et attend un clic
 * @param img : l'image à afficher
 * @param posX, posY : coordonnées spatiales pour positionner affichage de la fenetre sur l'écran
 * @param title : titre de la fenetre graphique
 * @warning : fonction bloquante
 */
void afficheImage(vpImage<unsigned char> img, int posX, int posY, const char *title)
{
    vpDisplayX d(img, posX, posY, title);
    vpDisplay::display(img);
    vpDisplay::flush(img);
    vpDisplay::getClick(img);
    vpDisplay::close(img);
}



// Fonction d'affichage d'une image sur sortie standard
void affiche(const vpImage<unsigned char> &src){
    int i,j;
    printf("\n ");
    for(i=0;i<src.getHeight();i++){
        for(j=0;j<src.getWidth();j++){
            printf("%d ",src[i][j]);
        }
        printf("\n ");
    }
}




/**
 * @brief calcule la rotation de k x pi/4 du gabarit
 * @param gabarit: gabarit d'entrée
 * @param k: parametre de la rotation (k x pi/4)
 * @return gabarit apres rotation de k x pi/4 (nouvelle image, allocation memoire)
 */
vpImage<unsigned char> rotation_masque(const vpImage<unsigned char> &gabarit, int k)
{
    
    vpImage<unsigned char> rotated(gabarit.getHeight(),gabarit.getWidth(),0) ;
    
    int half_h=gabarit.getHeight()/2;
    int half_w=gabarit.getWidth()/2;
    int offseti, offsetj;
    
    for(int i=-half_h;i<=half_h;i++){
        for(int j=-half_h;j<=half_h;j++){
            offseti=round(i*cos(k*M_PI/4)-j*sin(k*M_PI/4));
            offsetj=round(i*sin(k*M_PI/4)+j*cos(k*M_PI/4));
            rotated[offseti+half_h][offsetj+half_h]=gabarit[i+half_h][j+half_h];
        }
    }
    return rotated;
}



// La fonction rectangle cree une matrice binaire de taille n_l*n_c a fond noir (0) avec un rectangle
// blanc (255) dont le coin haut gauche est donne par les coordonnees i et j, et de
//largeur et longueur respectives lx et ly

vpImage<unsigned char> rectangle(int n_l, int n_c, int i, int j, int lx, int ly){
    
    vpImage<unsigned char> rect(n_l,n_c,0) ;
    int k,l;
    for(k=i;k<i+lx;k++){
        for(l=j;l<j+ly;l++){
            rect[k][l] = 255;
        }
    }
    return rect;
    
}



// Cette fonction renvoit 1 (255) si le gabarit de la transformation en tout-ou-rien
// peut etre "placé" au pixel (i,j).
int ttourien(const vpImage< unsigned char > &im, const vpImage< unsigned char > &gabarit, int i, int j){
    for(int imask=-1 ; imask<=1 ; imask++){
        for(int jmask=-1 ; jmask<=1 ; jmask++){
            
            if(i+imask >= 0 && i+imask < im.getHeight() 
            && j+jmask >= 0 && j+jmask < im.getWidth()){

                if(gabarit[imask+1][jmask+1] != im[i+imask][j+jmask] 
                && gabarit[imask+1][jmask+1] != 128) {

                    return 0;

                }
            }
        }
    }
    return 1;
}


// Cette fonction calcule la transformee en tout-ou-rien de l'image I par le gabarit 'gabarit'
bool  toutourien(const vpImage< unsigned char > &I, const vpImage< unsigned char > &gabarit, vpImage< unsigned char > &Ittr)
{
    bool modif = false;
    for(int i=0; i<I.getHeight();i++){
        for(int j=0;j<I.getWidth();j++){
            if(ttourien(I,gabarit,i,j)){
                Ittr[i][j] = 0;
                modif = true;
            }
        }
    }
    return modif;
}


/**
 * @brief effectue une squelettisation
 *
 * @param im: image de travail
 * @param gabarit: lettre L de l'alphabet de Golay
 *
 */
void squelette(vpImage< unsigned char > & im, const vpImage< unsigned char > & gabarit)
{
    vpImage< unsigned char >Ittr(im);
    vpImage< unsigned char >gab_famille[8];
    int it = 0;

    gab_famille[0]=gabarit;
    for (int k=1; k<8; k++){
        gab_famille[k]=rotation_masque(gabarit,k);
    }

    bool modif;
    while(true){ 
        modif=false;
        for (int k=0; k<8; k++){
            im = Ittr;
            modif = toutourien(im,gab_famille[k],Ittr) || modif;
            it++;
        }
        if(!modif){
            break;
        }
    }
    cout << "Nombre d'itérations : " << it << endl;
    afficheImage(Ittr,100,100,"Squelette");
}


vpImage<unsigned char> distance(vpImage<unsigned char> &im, vpImage<unsigned char> &mask) {
    vpImage<unsigned char> Idist(im);

    // Balayage direct
    for(int i = 0; i < Idist.getHeight(); i++) {
        for(int j = 0; j < Idist.getWidth(); j++) {
            if(Idist[i][j] == 255) {
                Idist[i][j] = 0;  
            } else {
                Idist[i][j] = 255;  
                
                if (i > 0 && mask[1][0] == 255)
                    Idist[i][j] = min((int)Idist[i][j], Idist[i-1][j] + 1); 
                if (j > 0 && mask[0][1] == 255) 
                    Idist[i][j] = min((int)Idist[i][j], Idist[i][j-1] + 1); 
                if (i > 0 && j > 0 && mask[0][0] == 255)
                    Idist[i][j] = min((int)Idist[i][j], Idist[i-1][j-1] + 1); 
                if (i > 0 && j < Idist.getWidth() - 1 && mask[0][2] == 255)
                    Idist[i][j] = min((int)Idist[i][j], Idist[i-1][j+1] + 1); 
            }
        }
    }

    // Balayage rétrograde
    for(int i = Idist.getHeight() - 1; i >= 0; i--) {
        for(int j = Idist.getWidth() - 1; j >= 0; j--) {
            if (i < Idist.getHeight() - 1 && mask[1][2] == 255) Idist[i][j] = min((int)Idist[i][j], Idist[i+1][j] + 1);
            if (j < Idist.getWidth() - 1 && mask[2][1] == 255) Idist[i][j] = min((int)Idist[i][j], Idist[i][j+1] + 1);
            if (i < Idist.getHeight() - 1 && j < Idist.getWidth() - 1 && mask[2][2] == 255)
                Idist[i][j] = min((int)Idist[i][j], Idist[i+1][j+1] + 1);
            if (i < Idist.getHeight() - 1 && j > 0 && mask[2][0] == 255) 
                Idist[i][j] = min((int)Idist[i][j], Idist[i+1][j-1] + 1);
        }
    }
     
    afficheImage(Idist,100,100,"Transformee en distance") ;
    return Idist;
}


void inverseImage(vpImage<unsigned char> &im) {
    for (int i = 0; i < im.getHeight(); i++) {
        for (int j = 0; j < im.getWidth(); j++) {
            im[i][j] = (im[i][j] == 0) ? 255 : 0;
        }
    }
    afficheImage(im,100,100,"Image inversee") ;
}

void binarise(vpImage<unsigned char> &im, int seuil){
    for(int i = 0; i < im.getHeight(); i++) {
        for(int j = 0; j < im.getWidth(); j++) {
            im[i][j] = (im[i][j] >= seuil) ? 255 : 0;
        }
    }
    afficheImage(im,100,100,"Image binarisee") ;
}

void connexe(vpImage<unsigned char> im){
    int taille = 9;

    for(int i = taille; i < im.getHeight()-taille; i++) {
        for(int j = taille; j < im.getWidth()-taille; j++) {
            bool cond = false;
            for(int imask = -taille; imask <= taille; imask++) {
                for(int jmask = 1; jmask <= taille; jmask++) {
                    cond = im[i+imask][j+jmask] || cond;
                }
            }
            for(int imask = 1; imask <= taille; imask++) {
                for(int jmask = -taille; jmask <= taille; jmask++) {
                    cond = im[i+imask][j+jmask] || cond;
                }
            }
            if(cond){
                im[i][j] = 0;            
            }
        }
    }
    afficheImage(im,100,100,"Image connexe") ;
    
    int cpt = 0 ;
    for(int i = taille; i < im.getHeight()-taille; i++) {
        for(int j = taille; j < im.getWidth()-taille; j++) {
            if(im[i][j] == 255) cpt++ ;
        }
    }

    cout << "Nombre d'éléments : " << cpt << endl ;
}



int main(int argc, char **argv)
{

    cout << "TIA - MORPHOLOGIE " << endl ;
    cout << "--" << endl ;

    int seuil=0;

    vpImage<unsigned char>  I0;
    vpImage<unsigned char>  L(3,3,0);

    vpImage<unsigned char>  Voisin4(3,3,0);
    vpImage<unsigned char>  Voisin8(3,3,255);

    vpImage<unsigned char> Iinv;
    vpImage<unsigned char> Idistance;
    vpImage<unsigned char> Ibinaire ;
    vpImage<unsigned char> Igraine ;
        
    vpImageIo::read(I0,"../images/test_operateur_bin.pgm");

    vpImage<unsigned char> Irec=rectangle(500,500,100, 100, 300, 300);

        
    // lettre L de l'alphabet de Golay
    L[0][0] = 255; L[0][1] = 255; L[0][2] = 255;
    L[1][0] = 128; L[1][1] = 255; L[1][2] = 128;
    L[2][0] = 0; L[2][1] = 0; L[2][2] = 0;

    // définition du voisin 4
    Voisin4[0][1] = 255; 
    Voisin4[1][0] = 255; Voisin4[1][1] = 255; Voisin4[1][2] = 255; 
    Voisin4[2][1] = 255; 

    int choix = 0;
    
    while(choix<5){
        cout << "1. Squelettisation" << endl;
        cout << "2. Transformee en distance" << endl;
        cout << "3. Erosion par transformee en distance" << endl;
        cout << "4. Problème" << endl;
        cout << "5. Sortir" << endl;
        cout << "Entrer numéro : "; 
        cin >> choix ; cout << endl;
        
        switch(choix){
            case 1 : 
                afficheImage(I0,100,100,"Image originale") ;
                squelette(I0,L);

                afficheImage(Irec,100,100,"Rectangle") ;
                squelette(Irec,L);
                break;
            
            case 2 :
                afficheImage(Irec,100,100,"Rectangle") ;
                Idistance = distance(Irec,Voisin4);
                Idistance = distance(Irec,Voisin8);

                afficheImage(I0,100,100,"Image originale") ;
                Idistance = distance(I0,Voisin4);
                Idistance = distance(I0,Voisin8);
                
                break;

            case 3 :
                afficheImage(Irec,100,100,"Rectangle") ;
                inverseImage(Irec);

                Idistance = distance(Irec,Voisin4);

                cout << "Seuil d'érosion : " ; cin >> seuil ; cout << endl;
                binarise(Idistance, seuil);
                break;

            case 4 :
                vpImageIo::read(Igraine,"../images/grains.png");
                afficheImage(Igraine,100,100,"Graines") ;
                binarise(Igraine, 124);
                squelette(Igraine,L);
                connexe(Igraine);

                break;

            default :
                break;
        }
    }

    cout << "Fin du programme " << endl ;
    return(0);
}















