/*
 * Copyright (c) 2013-2016 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 *
 */

#ifndef OPENBMP_ADDPATHDATACONTAINER_H
#define OPENBMP_ADDPATHDATACONTAINER_H

#include "BMPReader.h"
#include "bgp_common.h"


class AddPathDataContainer {
private:

    struct sendReceiveCodesForSentAndReceivedOpenMessageStructure {
        int     sendReceiveCodeForSentOpenMessage = 0;
        int     sendReceiveCodeForReceivedOpenMessage = 0;
    };

    // Peer related data container. First key is afi safi unique key. Second is structure with Add Path information
    typedef std::map<std::string, sendReceiveCodesForSentAndReceivedOpenMessageStructure> addPathMapType;

    // Persistent data container. First key is peer_info pointer, second is data for that peer
    static std::map<size_t, addPathMapType> peerInfoToAddPathMap;

    // Peer related information about Add Path
    addPathMapType *addPathMap;

    /**
     * Generates unique string from AFI and SAFI combination
     *
     * \param [in] afi              Afi code from RFC
     * \param [in] safi             Safi code form RFC
     *
     * \return string unique for AFI and SAFI combination
     */
    std::string getAFiSafiKeyString(int afi, int safi);

public:
    /**
     * Constructor for class
     *
     * \details Constructs peer related object with Add Path data.
     *
     * \param [in] peer_info   Persistent peer information
     */
    AddPathDataContainer(BMPReader::peer_info *peer_info);

    /**
     * Add Add Path data to persistent storage
     *
     * \param [in] afi              Afi code from RFC
     * \param [in] safi             Safi code form RFC
     * \param [in] send_receive     Send Recieve code from RFC
     * \param [in] sent_open        Is obtained from sent open message. False if from recieved
     */
    void addAddPath(int afi, int safi, int send_receive, bool sent_open);

    /**
     * Is add path capability enabled for such AFI and SAFI
     *
     * \param [in] afi              Afi code from RFC
     * \param [in] safi             Safi code form RFC
     *
     * \return is enabled
     */
    bool isAddPathEnabled(int afi, int safi);

};


#endif //OPENBMP_ADDPATHDATACONTAINER_H
