/*
 *  The RSA public-key cryptosystem
 *
 *  Copyright (C) 2006-2014, ARM Limited, All Rights Reserved
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*
 *  RSA was designed by Ron Rivest, Adi Shamir and Len Adleman.
 *
 *  http://theory.lcs.mit.edu/~rivest/rsapaper.pdf
 *  http://www.cacr.math.uwaterloo.ca/hac/about/chap8.pdf
 */
/*
 * lightjbl
 * Copyright (C) 2017 privatelo
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "rsa.h"

#include <string.h>

#if !defined(__OpenBSD__)
#include <stdlib.h>
#endif

#include <stdio.h>
#define polarssl_printf printf
#define polarssl_malloc malloc
#define polarssl_free   free

#define RSA_SIGN        1

/*
 * Initialize an RSA context
 */
void rsa_init( rsa_context *ctx)
{
    memset( ctx, 0, sizeof( rsa_context ) );
}

/*
 * Do an RSA private key operation
 */
int rsa_private( rsa_context *ctx,
                const unsigned char *input,
                unsigned char *output )
{
    int ret;
    size_t olen;
    mpi T, T1, T2;
    
    mpi_init( &T ); mpi_init( &T1 ); mpi_init( &T2 );
    
    MPI_CHK( mpi_read_binary( &T, input, ctx->len ) );
    if( mpi_cmp_mpi( &T, &ctx->N ) >= 0 )
    {
        ret = POLARSSL_ERR_MPI_BAD_INPUT_DATA;
        goto cleanup;
    }
    
    MPI_CHK( mpi_exp_mod( &T, &T, &ctx->D, &ctx->N, &ctx->RN ) );
    
    olen = ctx->len;
    MPI_CHK( mpi_write_binary( &T, output, olen ) );
    
cleanup:
    
    mpi_free( &T ); mpi_free( &T1 ); mpi_free( &T2 );
    
    if( ret != 0 )
        return( POLARSSL_ERR_RSA_PRIVATE_FAILED + ret );
    
    return( 0 );
}

/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PKCS1-V1_5-SIGN function
 */
/*
 * Do an RSA operation to sign the message digest
 */
int rsa_sign( rsa_context *ctx,
             const unsigned char *hash,
             unsigned char *sig )
{
    size_t nb_pad, olen;
    unsigned char *p = sig;
    int hashlen, oid_size;
    int ret;
    const char oid[] = "\x30\x20\x30\x0c\x06\x08\x2a\x86\x48\x86\xf7\x0d\x02\x05\x05\x00\x04\x10";
    
    olen = ctx->len;
    nb_pad = olen - 3;
    hashlen = 16;
    oid_size = 18;
    
    nb_pad -= oid_size;
    nb_pad -= hashlen;
    
    if( ( nb_pad < 8 ) || ( nb_pad > olen ) )
        return( POLARSSL_ERR_RSA_BAD_INPUT_DATA );

    *p++ = 0;
    *p++ = RSA_SIGN;
    memset( p, 0xFF, nb_pad );
    p += nb_pad;
    *p++ = 0;
    memcpy( p, oid, oid_size );
    p += oid_size;
    memcpy( p, hash, hashlen );
    MPI_CHK( rsa_private( ctx, sig, sig ) );
cleanup:
    return ret;
}

/*
 * Free the components of an RSA key
 */
void rsa_free( rsa_context *ctx )
{
    mpi_free( &ctx->Vi ); mpi_free( &ctx->Vf );
    mpi_free( &ctx->RQ ); mpi_free( &ctx->RP ); mpi_free( &ctx->RN );
    mpi_free( &ctx->QP ); mpi_free( &ctx->DQ ); mpi_free( &ctx->DP );
    mpi_free( &ctx->Q  ); mpi_free( &ctx->P  ); mpi_free( &ctx->D );
    mpi_free( &ctx->E  ); mpi_free( &ctx->N  );
}
