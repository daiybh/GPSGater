#ifndef CDataChannel_h_a29c75c2_d165_460e_b25f_ef12ff65c578
#define CDataChannel_h_a29c75c2_d165_460e_b25f_ef12ff65c578

class ITsDeviceMan;
class ITSAnalyzeMan;
class ITS_Communication;


    /** @author daili
     *  @version 0.1
     */
class CDataChannel {
    /* {src_lang=cpp}*/


 public:

    CDataChannel();

    virtual int Init();

    virtual void Unint();

    virtual int StartWork();

    virtual int StopWork();

 public:

    ITsDeviceMan *m_pDeviceMan;

    ITSAnalyzeMan *m_pAnalyzeMan;

    ITS_Communication *m_pCommunication;
};

#endif // CDataChannel_h_a29c75c2_d165_460e_b25f_ef12ff65c578

