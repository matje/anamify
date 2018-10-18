#include <ldns/ldns.h>
#include <ldns/host2str.h>
#include <stdio.h>


int
query_targets(ldns_resolver* res,
              const ldns_rdf* qname,
              const ldns_rr* aname,
              ldns_rr_type qtype,
              ldns_zone* z) {

    int j = 0;
    ldns_pkt* pkt = NULL;
    ldns_rr_list* rr_list = NULL;
    ldns_status s = ldns_resolver_query_status(&pkt, res, qname, qtype,
        LDNS_RR_CLASS_IN,
        LDNS_RD);

    if (s != LDNS_STATUS_OK) {
        fprintf(stderr, "Error sending query: %s",
        ldns_get_errorstr_by_id(s));
        return EXIT_FAILURE;
    }
    if (!pkt)  {
        fprintf(stderr, "No packet received");
        return EXIT_FAILURE;
    }

    rr_list = ldns_pkt_answer(pkt);
    if (ldns_rr_list_rr_count(rr_list) > 0) {
        for (j = 0; j < ldns_rr_list_rr_count(rr_list); j++) {
            ldns_rr_set_owner(ldns_rr_list_rr(rr_list, j),
                ldns_rr_owner(aname));
            ldns_rr_set_ttl(ldns_rr_list_rr(rr_list, j), ldns_rr_ttl(aname));
        }
        if (!ldns_zone_push_rr_list(z, rr_list)) {
            fprintf(stderr, "Failed to push new records to zone");
            return EXIT_FAILURE;
        }
    }

    /* TODO: This needs better error handling */

    return EXIT_SUCCESS;
}


int
main(int argc, char **argv)
{
    ldns_status s;
    int exit_status = EXIT_SUCCESS;

    /* Zone handling */
    char *filename = argv[1];
    FILE *fp;
    ldns_zone *z;
    int line_nr = 0;
    int i = 0;
    ldns_rr *rr = NULL;
    ldns_rr *aname = NULL;

    /* Resolver stuff */
    ldns_resolver *res = NULL;
    ldns_rdf *qname = NULL;
    ldns_pkt *pkt = NULL;

    /* TODO: This needs better option handling */
    if (argc != 2) {
        fprintf(stderr, "Bad argcount, expected: %s filename\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Read zone */
    fprintf(stderr, "Read zone %s\n", filename);
    filename = argv[1];
    fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Unable to open %s: %s\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }
    s = ldns_zone_new_frm_fp_l(&z, fp, NULL, 0, LDNS_RR_CLASS_IN, &line_nr);
    fclose(fp);
    if (s != LDNS_STATUS_OK) {
         fprintf(stderr, "%s at line %d\n",
             ldns_get_errorstr_by_id(s),
             line_nr);
         exit(EXIT_FAILURE);
    }

    /* Create resolver */
    s = ldns_resolver_new_frm_file(&res, NULL);
    if (s != LDNS_STATUS_OK) {
        fprintf(stderr, "Could not create a resolver structure: %s\n",
            ldns_get_errorstr_by_id(s));
        exit_status = EXIT_FAILURE;
        goto nogo;
    }

    /* Walk zone, search for ANAME */
    for (i = 0; i < ldns_rr_list_rr_count(ldns_zone_rrs(z)); i++) {
        aname = ldns_rr_list_rr(ldns_zone_rrs(z), i);

        if (65533 == ldns_rr_get_type(aname)) {
            /* Query for target address records */

            qname = ldns_rr_rdf(aname, 0);
            if (!qname) {
                fprintf(stderr, "Error in getting target qname");
                exit_status = EXIT_FAILURE;
                goto nogo;
            }
            ldns_rdf_set_type(qname, LDNS_RDF_TYPE_DNAME);

            exit_status = query_targets(res, qname, aname, LDNS_RR_TYPE_A, z);
            if (exit_status == EXIT_SUCCESS) {
                exit_status = query_targets(res, qname, aname,
                    LDNS_RR_TYPE_AAAA, z);
            }
            if (exit_status != EXIT_SUCCESS) {
                goto nogo;
            }

        }
    }

    /* Write zone */
    fprintf(stderr, "\n\nNew zone contents:\n");
    if (ldns_zone_soa(z)) {
        ldns_rr_print_fmt(stdout, ldns_output_format_default,
            ldns_zone_soa(z));
    }
    ldns_rr_list_print_fmt(stdout, ldns_output_format_default,
        ldns_zone_rrs(z));

nogo:

    exit(exit_status);
}

